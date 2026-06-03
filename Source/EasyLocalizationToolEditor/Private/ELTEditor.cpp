// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "ELTEditor.h"
#include "Internationalization/TextLocalizationResource.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Internationalization/StringTableCore.h"
#include "Internationalization/StringTable.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "ELTEditorSettings.h"
#include "ELTEditorWidget.h"
#include "ELTSettings.h"
#include "UObject/SavePackage.h"

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 1))
#include "AssetRegistry/AssetRegistryModule.h"
#else
#include "AssetRegistryModule.h"
#endif

#include "EditorUtilityWidget.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"

#include "CSVReader.h"
#include "LevelEditor.h"

#if ELTEDITOR_USE_SLATE_EDITOR_UI
#include "SELTEditorWidget.h"
#endif

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

DEFINE_LOG_CATEGORY(ELTEditorLog);

void UELTEditor::Init()
{
	// Load and cache paths for localization directories and CSVs.
	CSVPaths		= UELTEditorSettings::GetCSVPaths();
	CurrentLocPath	= UELTEditorSettings::GetLocalizationPath();

	// Reimport localizations (if this option is enabled).
	if (UELTEditorSettings::GetReimportAtEditorStartup())
	{
		FString OutMessage;
		GenerateLocFiles(OutMessage);
	}

	// Refresh information about available localizations.
	RefreshAvailableLangs(false);

	// Set preview language (if the option is enabled).
	SetLanguagePreview();
}

void UELTEditor::SetEditorTab(const TSharedRef<SDockTab>& NewEditorTab)
{
	EditorTab = NewEditorTab;
}

UEditorUtilityWidgetBlueprint* UELTEditor::GetUtilityWidgetBlueprint()
{
	// Get the Editor Utility Widget Blueprint from the content directory.
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 1))
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath("/EasyLocalizationTool/ELTEditorWidget_BP.ELTEditorWidget_BP"));
#else
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath("/EasyLocalizationTool/ELTEditorWidget_BP.ELTEditorWidget_BP");
#endif
	return Cast<UEditorUtilityWidgetBlueprint>(AssetData.GetAsset());
}

bool UELTEditor::CanCreateEditorUI()
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	return true;
#else
	// Editor UI can be created only when we have proper Editor Utility Widget Blueprint available.
	return GetUtilityWidgetBlueprint() != nullptr;
#endif
}

TSharedRef<SWidget> UELTEditor::CreateEditorUI()
{
	// Register OnMapChanged event so we can properly handle Tab and Widget when changing levels.
	FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditor.OnMapChanged().AddUObject(this, &UELTEditor::ChangeTabWorld);

	// Create the Widget
	return CreateEditorWidget();
}

TSharedRef<SWidget> UELTEditor::CreateEditorWidget()
{
	TSharedRef<SWidget> CreatedWidget = SNullWidget::NullWidget;

#if ELTEDITOR_USE_SLATE_EDITOR_UI
	EditorWidget = NewObject<UELTEditorWidget>(this);
	if (EditorWidget)
	{
		CreatedWidget = EditorWidget->GetWidget();
		InitializeTheWidget();
	}
#else
	if (UEditorUtilityWidgetBlueprint* UtilityWidgetBP = GetUtilityWidgetBlueprint())
	{
		// Create Widget from the Editor Utility Widget BP.
		CreatedWidget = UtilityWidgetBP->CreateUtilityWidget();

		// Save the pointer to the created Widget and initialize it.
		EditorWidget = Cast<UELTEditorWidget>(UtilityWidgetBP->GetCreatedWidget());
		if (EditorWidget)
		{
			InitializeTheWidget();
		}
	}
#endif

	// Returned Widget will be docked into the Editor Tab.
	return CreatedWidget;
}

void UELTEditor::ChangeTabWorld(UWorld* World, EMapChangeType MapChangeType)
{
	// Handle the event when editor map changes.
	if (MapChangeType == EMapChangeType::TearDownWorld)
	{
		// If the world is destroyed - set the Tab content to null and null the Widget.
		if (EditorTab.IsValid())
		{
			EditorTab.Pin()->SetContent(SNullWidget::NullWidget);
		}
		if (EditorWidget)
		{
			EditorWidget->Rename(nullptr, GetTransientPackage());
			EditorWidget = nullptr;
		}
	}
	else if (MapChangeType == EMapChangeType::NewMap || MapChangeType == EMapChangeType::LoadMap)
	{
		// If the map has been created or loaded and the Tab is valid - put a new Widget into this Tab.
		if (EditorTab.IsValid())
		{
			EditorTab.Pin()->SetContent(CreateEditorWidget());
		}
	}
}

void UELTEditor::InitializeTheWidget()
{
	// Check available languages (based on files in Localization directory)
	RefreshAvailableLangs(true);

	// Bind all required delegates to the Widget.
	EditorWidget->OnLocalizationPathSelectedDelegate.BindUObject(this, &UELTEditor::OnLocalizationPathChanged);
	EditorWidget->OnCSVPathChangedDelegate.BindUObject(this, &UELTEditor::OnCSVPathChanged);
	EditorWidget->OnGenerateLocFilesDelegate.BindUObject(this, &UELTEditor::OnGenerateLocFiles);
	EditorWidget->OnManuallySetLastLanguageChangedDelegate.BindUObject(this, &UELTEditor::OnManuallySetLastLanguageChanged);
	EditorWidget->OnReimportAtEditorStartupChangedDelegate.BindUObject(this, &UELTEditor::OnReimportAtEditorStartupChanged);
	EditorWidget->OnLocalizationPreviewChangedDelegate.BindUObject(this, &UELTEditor::OnLocalizationPreviewChanged);
	EditorWidget->OnLocalizationPreviewLangChangedDelegate.BindUObject(this, &UELTEditor::OnLocalizationPreviewLangChanged);
	EditorWidget->OnLocalizationOnFirstRunChangedDelegate.BindUObject(this, &UELTEditor::OnLocalizationFirstRunChanged);
	EditorWidget->OnLocalizationOnFirstRunLangChangedDelegate.BindUObject(this, &UELTEditor::OnLocalizationFirstRunLangChanged);
	EditorWidget->OnGlobalNamespaceChangedDelegate.BindUObject(this, &UELTEditor::OnGlobalNamespaceChanged);
	EditorWidget->OnSeparatorChangedDelegate.BindUObject(this, &UELTEditor::OnSeparatorChanged);
	EditorWidget->OnFallbackWhenEmptyChangedDelegate.BindUObject(this, &UELTEditor::OnFallbackWhenEmptyChanged);
	EditorWidget->OnGenerateKeyReferenceStringTableChangedDelegate.BindUObject(this, &UELTEditor::OnGenerateKeyReferenceStringTableChanged);
	EditorWidget->OnLogDebugChangedDelegate.BindUObject(this, &UELTEditor::OnLogDebugChanged);
	EditorWidget->OnPreviewInUIChangedDelegate.BindUObject(this, &UELTEditor::OnPreviewInUIChanged);

	// Fill Localization paths list on the Widget.
	TArray<FString> GameLocPaths = FPaths::GetGameLocalizationPaths();
	for (FString& GameLocPath : GameLocPaths)
	{
		GameLocPath = FPaths::ConvertRelativePathToFull(GameLocPath);
	}
	EditorWidget->CallFillLocalizationPaths(GameLocPaths);
	if (GameLocPaths.Num() > 0)
	{
		if (GameLocPaths.Contains(CurrentLocPath))
		{
			EditorWidget->CallSetLocalizationPath(CurrentLocPath);
		}
		else
		{
			EditorWidget->CallSetLocalizationPath(GameLocPaths[0]);
		}
	}

	// Set the Localization Preview current values to the Widget.
	EditorWidget->CallSetLocalizationPreview(UELTEditorSettings::GetLocalizationPreview());
	EditorWidget->CallSetLocalizationPreviewLang(UELTEditorSettings::GetLocalizationPreviewLang());

	// Set the ManualLastLanguageLoad current value to the Widget.
	EditorWidget->CallSetManuallySetLastUsedLanguage(UELTSettings::GetManuallySetLastUsedLanguage());

	// Set the ReimportAtEditorStartup current value to the Widget.
	EditorWidget->CallSetReimportAtEditorStartup(UELTEditorSettings::GetReimportAtEditorStartup());

	// Set the Localization Override At First Run current values to the Widget.
	EditorWidget->CallSetLocalizationOnFirstRun(UELTSettings::GetOverrideLanguageAtFirstLaunch());
	EditorWidget->CallSetLocalizationOnFirstRunLang(UELTSettings::GetLanguageToOverrideAtFirstLaunch());

	// Set the Generate Key Reference String Table current value to the Widget.
	EditorWidget->CallSetGenerateKeyReferenceStringTable(UELTEditorSettings::GetGenerateKeyReferenceStringTable());
	
	// Set LogDebug value to the Widget.
	EditorWidget->CallSetLogDebug(UELTEditorSettings::GetLogDebug());

	// Set PreivewInUI value to the Widget.
	EditorWidget->CallSetPreviewInUI(UELTEditorSettings::GetPreviewInUIEnabled());

	// Set Global Namespace value for this Localization directory to the Widget.
	const TMap<FString, FString>& GlobalNamespaces = UELTEditorSettings::GetGlobalNamespaces();
	if (GlobalNamespaces.Contains(GetCurrentLocName()))
	{
		EditorWidget->CallSetGlobalNamespace(GlobalNamespaces[GetCurrentLocName()]);
	}
	else
	{
		EditorWidget->CallSetGlobalNamespace(TEXT(""));
	}

	// Set Separator
	EditorWidget->CallSetSeparator(UELTEditorSettings::GetSeparator());

	// Set Fallback when Empty
	EditorWidget->CallSetFallbackWhenEmpty(UELTEditorSettings::GetFallbackWhenEmpty());
}


// ~~~~~~~~~ Events received from the Widget

void UELTEditor::OnLocalizationPathChanged(const FString& NewPath)
{
	// Localization directory path has been changed in the Widget. Update it in settings.
	CurrentLocPath = NewPath;
	UELTEditorSettings::SetLocalizationPath(CurrentLocPath);

	// Update Localization directory name and path to CSV to the Widget.
	EditorWidget->CallFillLocalizationName(GetCurrentLocName());
	EditorWidget->CallFillCSVPath(PathsStringToList(GetCurrentCSVPath()));

	// Refresh available languages for this Localization directory and set them to the Widget.
	RefreshAvailableLangs(false);
	EditorWidget->CallFillAvailableLangsInLocFile(CurrentAvailableLangsForLocFile);

	// Set Global Namespace for this Localization directory to the Widget.
	EditorWidget->CallSetGlobalNamespace(GetCurrentGlobalNamespace());
}

void UELTEditor::OnCSVPathChanged(const TArray<FString>& NewPaths)
{
	// CSV Path has been changed in the Widget. Cache it, save it in settings and update Widget.
	if (CSVPaths.Contains(GetCurrentLocName()))
	{
		CSVPaths[GetCurrentLocName()] = PathsListToString(RelativeToAbsolutePaths(NewPaths));
	}
	else
	{
		CSVPaths.Add(GetCurrentLocName(), PathsListToString(RelativeToAbsolutePaths(NewPaths)));
	}
	UELTEditorSettings::SetCSVPaths(CSVPaths);
	EditorWidget->CallFillCSVPath(PathsStringToList(GetCurrentCSVPath()));
}

void UELTEditor::OnGenerateLocFiles()
{
	// Generate Loc Files button has been pressed. 
	// Generate Loc Files and if succeeded refresh available languages and preview.
	FString ReturnMessage;
	const bool bSuccess = GenerateLocFiles(ReturnMessage);
	if (bSuccess)
	{
		RefreshAvailableLangs(true);
		SetLanguagePreview();
	}
	
	// Display a Dialog Window to inform user that the localization generation has been finished.
#if (ENGINE_MAJOR_VERSION == 5) && ENGINE_MINOR_VERSION >= 3
	FMessageDialog::Open((bSuccess ? EAppMsgCategory::Success : EAppMsgCategory::Error), EAppMsgType::Ok, FText::FromString(ReturnMessage));
#else
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ReturnMessage));
#endif
}

void UELTEditor::OnReimportAtEditorStartupChanged(bool bNewReimportAtEditorStartup)
{
	// "Reimport At Editor Startup" option has been changed in the Widget. Save this setting.
	UELTEditorSettings::SetReimportAtEditorStartup(bNewReimportAtEditorStartup);
}

void UELTEditor::OnLocalizationPreviewChanged(bool bNewLocalizationPreview)
{
	// "Localization Preview" option has been changed in the Widget. 
	// Save this setting, set proper preview.
	UELTEditorSettings::SetLocalizationPreview(bNewLocalizationPreview);
	SetLanguagePreview();
}

void UELTEditor::OnLocalizationPreviewLangChanged(const FString& LangPreview)
{
	// "Localization Preview Language" option has been changed in the Widget. 
	// Save this setting, set proper preview.
	UELTEditorSettings::SetLocalizationPreviewLang(LangPreview);
	SetLanguagePreview();
}

void UELTEditor::OnManuallySetLastLanguageChanged(bool bNewManuallySetLastLanguageLoad)
{
	// "Reimport At Editor Startup" option has been changed in the Widget. Save this setting.
	UELTSettings::SetManuallySetLastUsedLanguage(bNewManuallySetLastLanguageLoad);
}

void UELTEditor::OnLocalizationFirstRunChanged(bool bOnFirstRun)
{
	// "Override Language At First Launch" option has been changed in the Widget. Save this setting.
	UELTSettings::SetOverrideLanguageAtFirstLaunch(bOnFirstRun);
}

void UELTEditor::OnLocalizationFirstRunLangChanged(const FString& LangOnFirstRun)
{
	// "Language To Override At First Launch" option has been changed in the Widget. Save this setting.
	UELTSettings::SetLanguageToOverrideAtFirstLaunch(LangOnFirstRun);
}

void UELTEditor::OnGlobalNamespaceChanged(const FString& NewGlobalNamespace)
{
	// "Global Namespace" has been changed in the Widget. Save this setting 
	// for current Localization directory.
	TMap<FString, FString> GlobalNamespaces = UELTEditorSettings::GetGlobalNamespaces();
	if (GlobalNamespaces.Contains(GetCurrentLocName()) == false)
	{
		GlobalNamespaces.Add(GetCurrentLocName(), NewGlobalNamespace);
	}
	else
	{
		GlobalNamespaces[GetCurrentLocName()] = NewGlobalNamespace;
	}
	UELTEditorSettings::SetGlobalNamespace(GlobalNamespaces);
}

void UELTEditor::OnSeparatorChanged(const FString& NewSeparator)
{
	// "Separator" value has been changed in the Widget.
	// Clamp it to one character and save it to settings.
	FString Separator = NewSeparator;
	if (Separator.Len() == 0)
	{
		Separator = TEXT(",");
		EditorWidget->CallSetSeparator(Separator);
	}
	else if (Separator.Len() > 1)
	{
		Separator = FString(1, *Separator);
		EditorWidget->CallSetSeparator(Separator);
	}
	UELTEditorSettings::SetSeparator(Separator);
}

void UELTEditor::OnFallbackWhenEmptyChanged(const FString& NewFallback)
{
	UELTEditorSettings::SetFallbackWhenEmpty(NewFallback);
}

void UELTEditor::OnGenerateKeyReferenceStringTableChanged(bool bNewGenerateKeyReferenceStringTable)
{
	UELTEditorSettings::SetGenerateKeyReferenceStringTable(bNewGenerateKeyReferenceStringTable);
}

void UELTEditor::OnLogDebugChanged(bool bNewLogDebug)
{
	// "Log Debug" flag has been changed in the Widget. Save this setting.
	UELTEditorSettings::SetLogDebug(bNewLogDebug);
}

void UELTEditor::OnPreviewInUIChanged(bool bNewPreviewInUI)
{
	// "Preview In UI" option has been changed in the Widget. Save this setting.
	UELTEditorSettings::SetPreviewInUIEnabled(bNewPreviewInUI);
}

// ~~~~~~~~~ End of events received from the Widget


void UELTEditor::SetLanguagePreview()
{
	// Before enabling the preview for editor - disable it first in order to have a clear start.
	FTextLocalizationManager::Get().DisableGameLocalizationPreview();

	// Enable the preview for editor with a selected language, if this language is available.
	const FString& CurrentLang = UELTEditorSettings::GetLocalizationPreviewLang();
	if (UELTEditorSettings::GetLocalizationPreview() && CurrentAvailableLangs.Contains(CurrentLang))
	{
		FTextLocalizationManager::Get().EnableGameLocalizationPreview(CurrentLang);
	}
}

void UELTEditor::RefreshAvailableLangs(bool bRefreshUI)
{
	// Get all available languages by reading the localization directory.
	// Languages in current localization directory put into the separate array too.
	CurrentAvailableLangs.Empty();
	CurrentAvailableLangsForLocFile.Empty();
	const TArray<FString>& GameLocPaths = FPaths::GetGameLocalizationPaths();
	for (const FString& GameLocPath : GameLocPaths)
	{
		if (IFileManager::Get().DirectoryExists(*GameLocPath))
		{
			FString LocName = FPaths::GetBaseFilename(GameLocPath);
			TArray<FString> Dirs;
			IFileManager::Get().FindFiles(Dirs, *(FPaths::ConvertRelativePathToFull(GameLocPath) / TEXT("*")), false, true);
			for (FString& Dir : Dirs)
			{
				if (IFileManager::Get().FileExists(*(GameLocPath / Dir / LocName + TEXT(".locres"))))
				{
					CurrentAvailableLangs.AddUnique(Dir);
					if (LocName == GetCurrentLocName())
					{
						CurrentAvailableLangsForLocFile.AddUnique(Dir);
					}
				}
			}
		}
	}

	if (bRefreshUI)
	{
		// If the RefreshUI has been requested - set the available languages on the Widget.
		EditorWidget->CallFillAvailableLangs(CurrentAvailableLangs);
		EditorWidget->CallFillAvailableLangsInLocFile(CurrentAvailableLangsForLocFile);
		
		// If the Preview or OverrideAtFirstLaunch languages are not available after refreshing
		// available languages - set them to empty. If they are available - ensure they are properly displayed.
		// [TODO]: Shouldn't we disable them if they are not available? Test how it will behave.
		const FString LangPreview = UELTEditorSettings::GetLocalizationPreviewLang();
		const FString LangAtFirstLaunch = UELTSettings::GetLanguageToOverrideAtFirstLaunch();
		
		if (CurrentAvailableLangs.Contains(LangPreview) == false)
		{
			UELTEditorSettings::SetLocalizationPreviewLang(TEXT(""));
		}
		else
		{
			EditorWidget->CallSetLocalizationPreviewLang(LangPreview);
		}

		if (CurrentAvailableLangs.Contains(LangAtFirstLaunch) == false)
		{
			UELTSettings::SetLanguageToOverrideAtFirstLaunch(TEXT(""));
		}
		else
		{
			EditorWidget->CallSetLocalizationOnFirstRunLang(LangAtFirstLaunch);
		}
	}

	// Set available languages to the game settings.
	UELTSettings::SetAvailableLanguages(CurrentAvailableLangs);
}

FString UELTEditor::GetStringTableName(const FString& LocName, const FString& Namespace)
{
	return FString::Printf(TEXT("ELT_KeyReferences_%s_%s"), *LocName, *Namespace);
}

// IMPORTANT.
// CSV must have the following structure:
// |Namespace|Key|lang-en|lang-pl|...|
// or
// |Key|lang-en|lang-pl|lang-...| (if used global namespace)
// Namespace and Key MUST be at the beginning.
// Everything with lang-... will be treated as a language
// Any other key/value will be ignored
bool UELTEditor::GenerateLocFiles(FString& OutMessage)
{
	if (CurrentLocPath.IsEmpty() || GetCurrentLocName().IsEmpty())
	{
		OutMessage = TEXT("The Localization Name or Path is Empty! Can't generate loc files.\nEnsure the [Internationalization] section in DefaultGame.ini has at least one LocalizationPath.");
		return false;
	}
	const TArray<FString>& CSVFilePaths = PathsStringToList(GetCurrentCSVPath());
	const FString LocPath = FPaths::ConvertRelativePathToFull(CurrentLocPath);
	return GenerateLocFilesImpl(CSVFilePaths, LocPath, GetCurrentLocName(), GetCurrentGlobalNamespace(), UELTEditorSettings::GetSeparator(), UELTEditorSettings::GetFallbackWhenEmpty(), UELTEditorSettings::GetGenerateKeyReferenceStringTable(), OutMessage);
}

bool UELTEditor::GenerateLocFilesImpl(const FString& CSVPaths, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, const FString& Separator, const FString& FallbackWhenEmpty, bool bGenerateStringTables, FString& OutMessage)
{
	return GenerateLocFilesImpl(PathsStringToList(CSVPaths), LocPath, LocName, GlobalNamespace, Separator, FallbackWhenEmpty, bGenerateStringTables, OutMessage);
}

// Define the type of behavior when the localized string in CSV is empty and the fallback value should be used. 
enum class EFallbackWhenEmptyType : uint8
{
	NONE,
	FIRST_LANG,
	KEY
};

bool UELTEditor::GenerateLocFilesImpl(const TArray<FString>& CSVPaths, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, const FString& Separator, const FString& FallbackWhenEmpty, bool bGenerateStringTables, FString& OutMessage)
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

	// Get the FallbackWhenEmpty type.
	EFallbackWhenEmptyType FallbackWhenEmptyType = EFallbackWhenEmptyType::NONE;
	if (FallbackWhenEmpty.Equals(TEXT("FIRST_LANG"), ESearchCase::IgnoreCase))
	{
		FallbackWhenEmptyType = EFallbackWhenEmptyType::FIRST_LANG;
	} 
	else if (FallbackWhenEmpty.Equals(TEXT("KEY"), ESearchCase::IgnoreCase))
	{
		FallbackWhenEmptyType = EFallbackWhenEmptyType::KEY;
	}

	// Prepare locmeta file name.
	const FString MetaFileName = LocPath / LocName + TEXT(".locmeta");
	
	// Cache the info if we want to print debug logs.
	const bool bLogDebug = UELTEditorSettings::GetLogDebug();
	
	// Prepare containers for localization informations we will use later.
	TMap<FString, FTextLocalizationResource> LocReses; // Actual LocRes for each language.
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
			UE_LOG(ELTEditorLog, Log, TEXT("Parsing file: %s"), *CSVFilePath);
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
					if (KeysColumn == INDEX_NONE)
					{
						OutMessage = FString::Printf(TEXT("ERROR: Invalid CSV structure in file (%s)! Language column found before key column!"), *(CSVPaths[CSVIdx]));
						return false;
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

		// Ensure namespace/devnotes (if present) are located before the key column.
		if ((NamespaceColumn != INDEX_NONE && NamespaceColumn > KeysColumn) ||
			(DevNotesColumn != INDEX_NONE && DevNotesColumn > KeysColumn))
		{
			OutMessage = TEXT("ERROR: Invalid CSV! The 'namespace' and 'devnotes' columns must be before the 'key' column!");
			return false;
		}

		// Check if there are more columns after keys column. If not - we don't have any language columns and we can't generate loc files.
		if (Columns.Num() <= KeysColumn + 1)
		{
			OutMessage = TEXT("ERROR: Invalid CSV! There are no lang columns after key column!");
			return false;
		}

		// Validate if all columns have the same number of values. If not - we have invalid CSV structure and we can't generate loc files.
		const int32 NumOfValues = Columns[KeysColumn].Values.Num();
		for (int32 CIdx = KeysColumn + 1; CIdx < Columns.Num(); CIdx++)
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
			UE_LOG(ELTEditorLog, Log, TEXT("Adding Entries"));
			UE_LOG(ELTEditorLog, Log, TEXT("[Lang] | [Namespace] | [Key] | [Value]"));
		}

		// Prepare to cache the index of the first language column. We will need it if the FallbackWhenEmptyType is set to FIRST_LANG.
		int32 FirstLangColumn = INDEX_NONE;

		// Go through all language columns and add entries to proper LocRes.
		for (int32 Column = KeysColumn + 1; Column < Columns.Num(); Column++)
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

			// This is our first language column, cache its index.
			if (FirstLangColumn == INDEX_NONE)
			{
				FirstLangColumn = Column;
			}

			// Add LocRes for this language if it doesn't exist yet.
			if (LocReses.Contains(Lang) == false)
			{
				LocReses.Add(Lang, FTextLocalizationResource());
			}
			FTextLocalizationResource& LocRes = LocReses[Lang];

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
				if (FallbackWhenEmptyType != EFallbackWhenEmptyType::NONE)
				{
					if (LocalizedString.TrimStartAndEnd().IsEmpty())
					{
						if (FallbackWhenEmptyType == EFallbackWhenEmptyType::FIRST_LANG)
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
						else if (FallbackWhenEmptyType == EFallbackWhenEmptyType::KEY)
						{
							LocalizedString = Keys.Values[Key];
						}
					}
				}
				
				if (bLogDebug)
				{
					UE_LOG(ELTEditorLog, Log, TEXT("%s | %s | %s | %s"), *Lang, *Namespace, *(Keys.Values[Key]), *LocalizedString);
				}

				// Finally, we can add the LocRes entry!
				LocRes.AddEntry(
					FTextKey(Namespace),
					FTextKey(Keys.Values[Key]),
					Keys.Values[Key],
					LocalizedString,
					0);

				// If we want to generate string tables with key references - cache the key for this namespace. We will use it later to generate string tables.
				if (bGenerateStringTables && (Keys.Values[Key].IsEmpty() == false))
				{
					NamespaceToKeysMap.FindOrAdd(Namespace).Add(Keys.Values[Key]);
				}
			}
		}
	}

	// LocMeta must be created for every localization path.
	FTextLocalizationMetaDataResource LocMeta;
	LocMeta.NativeCulture = TEXT("en");
	LocMeta.NativeLocRes = TEXT("en") / LocName + TEXT(".locres");
	if (bLogDebug)
	{
		UE_LOG(ELTEditorLog, Log, TEXT("Saved Meta File: %s"), *MetaFileName);
	}
	LocMeta.SaveToFile(MetaFileName);

	for (auto& LocRes : LocReses)
	{
		const FString LocFileName = LocPath / LocRes.Key / LocName + TEXT(".locres");
		if (bLogDebug)
		{
			UE_LOG(ELTEditorLog, Log, TEXT("Saved Loc File: %s"), *LocFileName);
		}
		LocRes.Value.SaveToFile(LocFileName);
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
			UStringTable* StringTableAsset = NewObject<UStringTable>(Package, UStringTable::StaticClass(), FName(*AssetName), (RF_Public | RF_Standalone | RF_Transactional));
			if (StringTableAsset == nullptr)
			{
				OutMessage = FString::Printf(TEXT("ERROR: Failed to create StringTable asset: %s"), *AssetName);
				return false;
			}
			FAssetRegistryModule::AssetCreated(StringTableAsset);
			Package->MarkPackageDirty();

			// Setup StringTable asset with keys as source strings. We will use keys as localized strings too, so the value is the same as the key.
			// Clear the StringTable asset from any existing source strings before adding new ones, so we can properly update the existing asset on reimport.
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

				StringTableRef->SetSourceString(FTextKey(Key), Key, DevNotes);
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
				UE_LOG(ELTEditorLog, Log, TEXT("Saved String Table Asset: %s"), *PackageFileName);
			}
		}
	}

	OutMessage = TEXT("SUCCESS: Localization import complete!");
	return true;
}

const FString& UELTEditor::GetCurrentCSVPath()
{
	// Get the CSV path associated with current Localization directory.
	// If there is no such file - return empty value.
	if (CSVPaths.Contains(GetCurrentLocName()))
	{
		return CSVPaths[GetCurrentLocName()];
	}
	else
	{
		static FString Dummy = TEXT("");
		return Dummy;
	}
}

FString UELTEditor::GetCurrentLocName()
{
	// Get the base file name of current Localization directory name.
	if (CurrentLocPath.IsEmpty() == false)
	{
		return FPaths::GetBaseFilename(CurrentLocPath);
	}
	else
	{
		return TEXT("");
	}
}

FString UELTEditor::GetCurrentGlobalNamespace()
{
	// Get the Global Namespace associated with current Localization directory.
	// If there is no such Global Namespace - return empty value.
	const TMap<FString, FString>& GlobalNamespaces = UELTEditorSettings::GetGlobalNamespaces();
	if (GlobalNamespaces.Contains(GetCurrentLocName()))
	{
		return GlobalNamespaces[GetCurrentLocName()];
	}
	else
	{
		return TEXT("");
	}
}

TArray<FString> UELTEditor::RelativeToAbsolutePaths(const TArray<FString>& RelativePaths)
{
	TArray<FString> Result;
	for (const FString& RelativePath : RelativePaths)
	{
		Result.Add(FPaths::ConvertRelativePathToFull(RelativePath));
	}
	return Result;
}

FString UELTEditor::PathsListToString(const TArray<FString>& Paths)
{
	return FString::Join(Paths, TEXT(";"));
}

TArray<FString> UELTEditor::PathsStringToList(const FString& Paths)
{
	TArray<FString> Result;
	Paths.ParseIntoArray(Result, TEXT(";"));
	return Result;
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION