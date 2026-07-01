// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "ELTImporter.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTable.h"
#include "UObject/SavePackage.h"
#include "CSVReader.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Misc/Paths.h"

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 1))
#include "AssetRegistry/AssetRegistryModule.h"
#else
#include "AssetRegistryModule.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(ELTImporterLog, Log, All);

// Declaration of a static cache
TMap<FString, FTextLocalizationResource> FELTImporter::CachedResources = {};
int32 FELTImporter::CachedResourcesPriority = -1;

bool FELTImporter::GenerateLoc(	const TArray<FString>& CSVPaths,
								const FString& LocPath, 
								const FString& LocName,
								const FString& GlobalNamespace, 
								const FString& Separator,
								EFallbackWhenEmptyType FallbackWhenEmpty,
								bool bGenerateStringTables, 
								bool bSaveToFiles,
								bool bCache,
								bool bLogDebug, 
								FString& OutMessage)
{
	// Validate the input data first. If something is wrong - return false and set the OutMessage with the error description.
	if (CSVPaths.Num() == 0)
	{
		OutMessage = TEXT("ERROR: No CSV files provided! Please provide at least one CSV file to generate localization files.");
		return false;
	}

	// Validate the Separator value. It must be exactly 1 character.
	if (Separator.Len() != 1)
	{
		OutMessage = FString::Printf(TEXT("ERROR: The Separator is invalid. Must be exactly 1 character. Current Separator = %s"), *Separator);
		return false;
	}

	// Check if the Importer will do anything
	if ((bSaveToFiles == false) && (bCache == false) && (bGenerateStringTables == false))
	{
		OutMessage = TEXT("ERROR: Imported do not generate neither files nor cache nor string tables. What is it suppose to do?");
		return false;
	}

	// Prepare locmeta file name.
	const FString MetaFileName = bSaveToFiles ? (LocPath / LocName + TEXT(".locmeta")) : TEXT("");
	
	// Prepare containers for localization informations we will use later.
	TMap<FString, FTextLocalizationResource> LocReses; // Actual LocRes for each language. Will be used to save them to files.
	TMap<FString, TSet<FString>> NamespaceToKeysMap; // List of keys for each  namespace (used for generating String Tables).
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 8))
	TMap<FString, TMap<FString, FString>> NamespaceToKeysToNotesMap; // List of keys with dev notes for each namespace (used for generating String Tables with dev notes). 
#endif

	// Go thorugh all CVS files.
	for (int32 CSVIdx = 0; CSVIdx < CSVPaths.Num(); CSVIdx++)
	{
		// Get the full path to the CSV file.
		const FString CSVFilePath = FPaths::ConvertRelativePathToFull(CSVPaths[CSVIdx]);
		if (bLogDebug)
		{
			UE_LOG(ELTImporterLog, Log, TEXT("Parsing file: %s"), *CSVFilePath);
		}

		FCSVReader Reader;
		if (Reader.LoadFromFile(CSVFilePath, (*Separator)[0], OutMessage) == false)
		{
			OutMessage = FString::Printf(TEXT("ERROR: Failed to load CSV file (%s)! Error: %s"), *CSVFilePath, *OutMessage);
			return false;
		}

		// Get the Idx for the columns that has namespace, devnotes and keys. Validate if the CSV has proper structure.
		int32 NamespaceColumn = INDEX_NONE;
		int32 DevNotesColumn = INDEX_NONE;
		int32 KeysColumn = INDEX_NONE;
		int32 FirstLangColumn = INDEX_NONE;
		const TArray<FCSVColumn> Columns = Reader.Columns;
		for (int32 ColumnIdx = 0; ColumnIdx < Columns.Num(); ColumnIdx++)
		{
			if (Columns[ColumnIdx].Values.Num() == 0)
			{
				OutMessage = FString::Printf(TEXT("ERROR: The Column (%i) in CSV (%s) is empty!"), ColumnIdx, *(CSVPaths[CSVIdx]));
				return false;
			}

			FString Header = Columns[ColumnIdx].Values[0].TrimStartAndEnd();
			if (Header.Equals(TEXT("namespace"), ESearchCase::IgnoreCase))
			{
				if (NamespaceColumn != INDEX_NONE)
				{
					OutMessage = FString::Printf(TEXT("ERROR: Invalid CSV structure in file (%s)! Multiple 'namespace' columns found!"), *(CSVPaths[CSVIdx]));
					return false;
				}
				NamespaceColumn = ColumnIdx;
			}
			else if (Header.Equals(TEXT("devnotes"), ESearchCase::IgnoreCase))
			{
				if (DevNotesColumn != INDEX_NONE)
				{
					OutMessage = FString::Printf(TEXT("ERROR: Invalid CSV structure in file (%s)! Multiple 'devnotes' columns found!"), *(CSVPaths[CSVIdx]));
					return false;
				}
				DevNotesColumn = ColumnIdx;
			}
			else if (Header.Equals(TEXT("key"), ESearchCase::IgnoreCase))
			{
				if (KeysColumn != INDEX_NONE)
				{
					OutMessage = FString::Printf(TEXT("ERROR: Invalid CSV structure in file (%s)! Multiple 'key' columns found!"), *(CSVPaths[CSVIdx]));
					return false;
				}
				KeysColumn = ColumnIdx;
			}
			else
			{
				Header.ReplaceCharInline(TEXT('_'), TEXT('-'));
				if (Header.RemoveFromStart(TEXT("lang-"), ESearchCase::IgnoreCase))
				{
					if (FirstLangColumn == INDEX_NONE)
					{
						FirstLangColumn = ColumnIdx;
					}
				}
			}
		}

		// Make sure we have keys column.
		if (KeysColumn == INDEX_NONE)
		{
			OutMessage = TEXT("ERROR: Invalid CSV! Key column not found!");
			return false;
		}

		// Make sure we have first lang column.
		if (FirstLangColumn == INDEX_NONE)
		{
			OutMessage = TEXT("ERROR: Invalid CSV! No Lang column found!");
			return false;
		}

		// Ensure namespace/devnotes/keys are located before the lang column.
		if ((NamespaceColumn != INDEX_NONE && NamespaceColumn >= FirstLangColumn) ||
			(DevNotesColumn != INDEX_NONE && DevNotesColumn >= FirstLangColumn) ||
			(KeysColumn >= FirstLangColumn))
		{
			OutMessage = TEXT("ERROR: Invalid CSV! The 'namespace', 'devnotes', and 'key' columns must be before the 'lang' column!");
			return false;
		}

		// Validate if all columns have the same number of values. If not - we have invalid CSV structure and we can't generate loc files.
		const int32 NumOfValues = Columns[KeysColumn].Values.Num();
		for (int32 CIdx = FirstLangColumn; CIdx < Columns.Num(); CIdx++)
		{
			if (Columns[CIdx].Values.Num() != NumOfValues)
			{
				OutMessage = FString::Printf(TEXT("ERROR: Invalid CSV! Column %i (counting from 1) has %i values while Column 1 has %i values. Every Column must have the same amount of values!"), CIdx + 1, Columns[CIdx].Values.Num(), NumOfValues);
				return false;
			}
		}

		// Potential place for namespaces (if the column exists).
		const FCSVColumn& Namespaces = (NamespaceColumn != INDEX_NONE) ? Columns[NamespaceColumn] : FCSVColumn();

		// Check if we want to use global namespace (there is no namespace column and global namespace value is empty).
		const bool bUseGlobalNamespace = (NamespaceColumn == INDEX_NONE) && (GlobalNamespace.IsEmpty() == false);

		// We don't want to use global namespace but we don't have namespace column - it's an error.
		if (bUseGlobalNamespace == false && (NamespaceColumn == INDEX_NONE))
		{
			OutMessage = TEXT("ERROR: Namespaces in CSV not found!");
			return false;
		}

		// Potential place for devnotes.
		const FCSVColumn& DevNotes = (DevNotesColumn != INDEX_NONE) ? Columns[DevNotesColumn] : FCSVColumn();

		// Clear the localization directory first, preserving any .uasset files (e.g. string table assets). 
		// Deleting .uasset files while the corresponding UPackage is still in memory invalidates the async loader's package tracking and causes an assertion on the next reimport.
		// Delete files only if we are suppose to create new files.
		if (bSaveToFiles)
		{
			if (CSVIdx == 0)
			{
				// Ensure we are not deleting any important files by checking if we are in Content directory and the Meta file is there exists.
				if (LocPath.Contains("Content") && IFileManager::Get().FileExists(*MetaFileName))
				{
					TArray<FString> FilesToDelete;
					IFileManager::Get().FindFilesRecursive(FilesToDelete, *LocPath, TEXT("*"), true, false);
					for (const FString& File : FilesToDelete)
					{
						if (File.EndsWith(TEXT(".uasset")) == false)
						{
							IFileManager::Get().Delete(*File);
						}
					}
				}
			}
		}

		// Get the keys column. We know it's valid because we've already validated it.
		const FCSVColumn& Keys = Columns[KeysColumn];

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 8))
		// Gather Dev Notes if available
		if (DevNotesColumn != INDEX_NONE)
		{
			for (int32 Key = 1; Key < Keys.Values.Num(); Key++)
			{
				FString DevNote = DevNotes.Values[Key];
				if (DevNote.IsEmpty() == false)
				{
					const FString& Namespace = (bUseGlobalNamespace || Namespaces.Values[Key].IsEmpty()) ? GlobalNamespace : Namespaces.Values[Key];
					if (Namespace.IsEmpty())
					{
						OutMessage = FString::Printf(TEXT("ERROR: Namespace in row %i (counting from 1) for dev note is empty!"), Key);
						return false;
					}
					NamespaceToKeysToNotesMap.FindOrAdd(Namespace).Add(Keys.Values[Key], DevNote);
				}
			}
		}
#endif

		if (bLogDebug)
		{
			UE_LOG(ELTImporterLog, Log, TEXT("Adding Entries"));
			UE_LOG(ELTImporterLog, Log, TEXT("[Lang] | [Namespace] | [Key] | [Value]"));
		}

		// Go through all language columns and add entries to proper LocRes.
		for (int32 Column = FirstLangColumn; Column < Columns.Num(); Column++)
		{
			// Get the column with the localized values.
			const FCSVColumn& Locs = Columns[Column];

			// Read the language code. We know the number of values in this column is the same as in keys column because we've already validated it.
			FString Lang = Locs.Values[0].ToLower();

			// Replace underscores with hyphens to match the expected format.
			Lang.ReplaceCharInline('_', '-');

			// Remove "lang-" prefix if exists to get the actual language code.
			// If the prefix is not there - it's an invalid CSV structure, because all language columns must start with "lang-" prefix.
			if (Lang.RemoveFromStart(TEXT("lang-")) == false)
			{
				continue;
			}

			// Add LocRes for this language if it doesn't exist yet.
			FTextLocalizationResource& LocRes = LocReses.FindOrAdd(Lang);

			// Add LocRes cache if the cache is required
			static FTextLocalizationResource DummyLocRes = {};
			FTextLocalizationResource& CachedLocRes = bCache ? CachedResources.FindOrAdd(Lang) : DummyLocRes;

			// For each key add the entry to the LocRes.
			for (int32 Key = 1; Key < Keys.Values.Num(); Key++)
			{
				// Get correct Namespace. 
				// If we want to use global namespace - use global namespace. 
				// If there is no namespace specified in this column - use global namespace.
				// If there is namespace specified - use it.
				// Getting namespace value is safe, because we've already validated that.
				const FString& Namespace = (bUseGlobalNamespace || Namespaces.Values[Key].IsEmpty()) ? GlobalNamespace : Namespaces.Values[Key];
				if (Namespace.IsEmpty())
				{
					OutMessage = FString::Printf(TEXT("ERROR: Namespace in row %i (counting from 1) is empty!"), Key);
					return false;
				}

				// If the localized string is empty and the fallback option is set - use the fallback value.
				FString LocalizedString = Locs.Values[Key];
				if (FallbackWhenEmpty != EFallbackWhenEmptyType::NONE)
				{
					if (LocalizedString.TrimStartAndEnd().IsEmpty())
					{
						if (FallbackWhenEmpty == EFallbackWhenEmptyType::FIRST_LANG)
						{
							// Use the first language column value as fallback.
							if (Columns.IsValidIndex(FirstLangColumn))
							{
								LocalizedString = Columns[FirstLangColumn].Values[Key];
							}

							// If the first language value is also empty - use the key as a fallback.
							if (LocalizedString.TrimStartAndEnd().IsEmpty())
							{
								LocalizedString = Keys.Values[Key];
							}
						}
						else if (FallbackWhenEmpty == EFallbackWhenEmptyType::KEY)
						{
							LocalizedString = Keys.Values[Key];
						}
					}
				}
				
				if (bLogDebug)
				{
					UE_LOG(ELTImporterLog, Log, TEXT("%s | %s | %s | %s"), *Lang, *Namespace, *(Keys.Values[Key]), *LocalizedString);
				}

				// Finally, we can add the LocRes entry!
				// We do this only if we will save them to file later.
				// In any other case they are not useable.
				if (bSaveToFiles)
				{
					LocRes.AddEntry(
						FTextKey(Namespace),
						FTextKey(Keys.Values[Key]),
						Keys.Values[Key],
						LocalizedString,
						0);
				}

				// Add cache. We keep cache and LocRes separate, because Cache 
				// needs to be incremental, while LocReses (which are then saved to file) 
				// are always constructed from scratch.
				if (bCache)
				{
					CachedLocRes.AddEntry(
						FTextKey(Namespace),
						FTextKey(Keys.Values[Key]),
						Keys.Values[Key],
						LocalizedString,
						CachedResourcesPriority);
				}

				// If we want to generate string tables with key references - cache the key for this namespace. We will use it later to generate string tables.
				if (bGenerateStringTables && (Keys.Values[Key].IsEmpty() == false))
				{
					NamespaceToKeysMap.FindOrAdd(Namespace).Add(Keys.Values[Key]);
				}
			}
		}
	}

	// Decrease the priority for cached resources, so that the next import 
	// will have a higher priority and will override the previous one.
	// (Lower value = higher priority)
	if (bCache)
	{
		--CachedResourcesPriority;
	}

	// LocMeta must be created for every localization path.
	FTextLocalizationMetaDataResource LocMeta;
	LocMeta.NativeCulture = TEXT("en");
	LocMeta.NativeLocRes = TEXT("en") / LocName + TEXT(".locres");
	
	if (bSaveToFiles)
	{
		LocMeta.SaveToFile(MetaFileName);
		if (bLogDebug)
		{
			UE_LOG(ELTImporterLog, Log, TEXT("Saved Meta File: %s"), *MetaFileName);
		}
		for (auto& LocRes : LocReses)
		{
			const FString LocFileName = LocPath / LocRes.Key / LocName + TEXT(".locres");
			if (bLogDebug)
			{
				UE_LOG(ELTImporterLog, Log, TEXT("Saved Loc File: %s"), *LocFileName);
			}
			LocRes.Value.SaveToFile(LocFileName);
		}
	}

	// Generate Key Reference String Table
	if (bGenerateStringTables && NamespaceToKeysMap.Num() > 0)
	{
		for (const auto& KVP : NamespaceToKeysMap)
		{
			const FString& Namespace = KVP.Key;
			const TSet<FString>& Keys = KVP.Value;

			const FString AssetName = GetStringTableName(LocName, Namespace);
			const FString PackagePath = FPackageName::FilenameToLongPackageName(LocPath / AssetName);

			// If the package is already in memory (e.g. from a previous reimport), use it directly.
			UPackage* Package = FindPackage(nullptr, *PackagePath);
			if (Package == nullptr)
			{
				// If the package is not in memory - check if it exists on disk. If it exists - load it, if not - create a new one.
				Package = FPackageName::DoesPackageExist(*PackagePath) ? LoadPackage(nullptr, *PackagePath, LOAD_None) : CreatePackage(*PackagePath);
			}

			// If we failed to find, load or create the package - return an error.
			if (Package == nullptr)
			{
				OutMessage = FString::Printf(TEXT("ERROR: Failed to create package path for StringTable: %s"), *PackagePath);
				return false;
			}
			
			UStringTable* StringTableAsset = FindObject<UStringTable>(Package, *AssetName);
			if (bSaveToFiles)
			{
				// Clear any existing StringTable that resides in memory before creating a new one.
				if (UStringTable* ExistingStringTableAsset = FindObject<UStringTable>(Package, *AssetName))
				{
					ExistingStringTableAsset->ClearFlags(RF_Public | RF_Standalone);
					if (ExistingStringTableAsset->IsRooted())
					{
						ExistingStringTableAsset->RemoveFromRoot();
					}
#if (ENGINE_MAJOR_VERSION == 5)
					ExistingStringTableAsset->MarkAsGarbage();
#else
					ExistingStringTableAsset->MarkPendingKill();
#endif
				}
			
				// Create new StringTable asset in memory. If we fail - return an error.
				StringTableAsset = NewObject<UStringTable>(Package, UStringTable::StaticClass(), FName(*AssetName), (RF_Public | RF_Standalone | RF_Transactional));
				if (StringTableAsset == nullptr)
				{
					OutMessage = FString::Printf(TEXT("ERROR: Failed to create StringTable asset: %s"), *AssetName);
					return false;
				}

				FAssetRegistryModule::AssetCreated(StringTableAsset);
				Package->MarkPackageDirty();
			}

			// Setup StringTable asset with keys as source strings. We will use keys as localized strings too, so the value is the same as the key.
			// Clear the StringTable asset from any existing source strings before adding new ones, so we can properly update the existing asset on reimport.
			if (StringTableAsset == nullptr)
			{
				OutMessage = FString::Printf(TEXT("ERROR: Failed to read or create StringTable asset: %s"), *AssetName);
				return false;
			}
			FStringTableRef StringTableRef = StringTableAsset->GetMutableStringTable();
			StringTableRef->SetNamespace(Namespace);
			StringTableRef->ClearSourceStrings();

#if (ENGINE_MAJOR_VERSION == 5)
	#if (ENGINE_MINOR_VERSION >= 8)
			// For UE5.8 and newer add source strings to the String Table alongside with dev notes if they are available in the CSV.
			TMap<FString, FString>* KeysToNotes = NamespaceToKeysToNotesMap.Find(Namespace);
			for (const FString& Key : Keys)
			{
				FString DevNotes = TEXT("");
				if (FString* Note = KeysToNotes ? KeysToNotes->Find(Key) : nullptr)
				{
					DevNotes = *Note;
				}
#if WITH_EDITORONLY_DATA
				StringTableRef->SetSourceString(FTextKey(Key), Key, DevNotes);
#else
				StringTableRef->SetSourceString(FTextKey(Key), Key);
#endif
			}
	#else
			// For UE5.0 - UE5.7 add source strings to the String Table without dev notes.
			for (const FString& Key : Keys)
			{
				StringTableRef->SetSourceString(FTextKey(Key), Key);
			}
	#endif
#else
			// For UE4 add source strings to the String Table, but with different function signature.
			for (const FString& Key : Keys)
			{
				StringTableRef->SetSourceString(Key, Key);
			}
#endif

			if (bSaveToFiles)
			{
				// Save the package with the StringTable asset to disk. If we fail - return an error.
				FString PackageFileName = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
				FSavePackageArgs SaveArgs;
				SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
				SaveArgs.Error = GError;

#if (ENGINE_MAJOR_VERSION == 5)
				if (UPackage::SavePackage(Package, StringTableAsset, *PackageFileName, SaveArgs) == false)
#else
				if (UPackage::SavePackage(Package, StringTableAsset, SaveArgs.TopLevelFlags, *PackageFileName, SaveArgs.Error) == false)
#endif
				{
					OutMessage = FString::Printf(TEXT("ERROR: Failed to save StringTable package file to disk path: %s"), *PackageFileName);
					return false;
				}

				if (bLogDebug)
				{
					UE_LOG(ELTImporterLog, Log, TEXT("Saved String Table Asset: %s"), *PackageFileName);
				}
			}
		}
	}

	OutMessage = TEXT("SUCCESS: Localization import complete!");
	return true;
}

FString FELTImporter::GetStringTableName(const FString& LocName, const FString& Namespace)
{
	return FString::Printf(TEXT("ELT_KeyReferences_%s_%s"), *LocName, *Namespace);
}

EFallbackWhenEmptyType FELTImporter::FallbackStringToEnum(const FString& String)
{
	if (String.Equals(TEXT("FIRST_LANG"), ESearchCase::IgnoreCase))
	{
		return EFallbackWhenEmptyType::FIRST_LANG;
	}
	if (String.Equals(TEXT("KEY"), ESearchCase::IgnoreCase))
	{
		return EFallbackWhenEmptyType::KEY;
	}
	return EFallbackWhenEmptyType::NONE;
}

FString FELTImporter::FallbackEnumToString(EFallbackWhenEmptyType Enum)
{
	if (Enum == EFallbackWhenEmptyType::FIRST_LANG)
	{
		return TEXT("FIRST_LANG");
	}
	if (Enum == EFallbackWhenEmptyType::KEY)
	{
		return TEXT("KEY");
	}
	return TEXT("NONE");
}
