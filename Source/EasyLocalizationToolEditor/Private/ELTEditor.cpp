// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#include "ELTEditor.h"
#include "Internationalization/TextLocalizationResource.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "ELTEditorSettings.h"
#include "ELTEditorWidget.h"
#include "ELTSettings.h"

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
	// Editor UI can be created only when we have proper Editor Utility Widget Blueprint available.
	return GetUtilityWidgetBlueprint() != nullptr;
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
	EditorWidget->OnLogGebugChangedDelegate.BindUObject(this, &UELTEditor::OnLogDebugChanged);

	// Fill Localization paths list on the Widget.
	TArray<FString> GameLocPaths = FPaths::GetGameLocalizationPaths();
	for (FString& GameLocPath : GameLocPaths)
	{
		GameLocPath = FPaths::ConvertRelativePathToFull(GameLocPath);
	}
	EditorWidget->FillLocalizationPaths(GameLocPaths);
	if (GameLocPaths.Num() > 0)
	{
		if (GameLocPaths.Contains(CurrentLocPath))
		{
			EditorWidget->SetLocalizationPath(CurrentLocPath);
		}
		else
		{
			EditorWidget->SetLocalizationPath(GameLocPaths[0]);
		}
	}

	// Set the Localization Preview current values to the Widget.
	EditorWidget->SetLocalizationPreview(UELTEditorSettings::GetLocalizationPreview());
	EditorWidget->SetLocalizationPreviewLang(UELTEditorSettings::GetLocalizationPreviewLang());

	// Set the ManualLastLanguageLoad current value to the Widget.
	EditorWidget->SetManuallySetLastUsedLanguage(UELTSettings::GetManuallySetLastUsedLanguage());

	// Set the ReimportAtEditorStartup current value to the Widget.
	EditorWidget->SetReimportAtEditorStartup(UELTEditorSettings::GetReimportAtEditorStartup());

	// Set the Localization Override At First Run current values to the Widget.
	EditorWidget->SetLocalizationOnFirstRun(UELTSettings::GetOverrideLanguageAtFirstLaunch());
	EditorWidget->SetLocalizationOnFirstRunLang(UELTSettings::GetLanguageToOverrideAtFirstLaunch());

	// Set LogDebug value to the Widget.
	EditorWidget->SetLogDebug(UELTSettings::GetLogDebug());

	// Set Global Namespace value for this Localization directory to the Widget.
	const TMap<FString, FString>& GlobalNamespaces = UELTEditorSettings::GetGlobalNamespaces();
	if (GlobalNamespaces.Contains(GetCurrentLocName()))
	{
		EditorWidget->SetGlobalNamespace(GlobalNamespaces[GetCurrentLocName()]);
	}
	else
	{
		EditorWidget->SetGlobalNamespace(TEXT(""));
	}

	// Set Separator
	EditorWidget->SetSeparator(UELTEditorSettings::GetSeparator());
}


// ~~~~~~~~~ Events received from the Widget

void UELTEditor::OnLocalizationPathChanged(const FString& NewPath)
{
	// Localization directory path has been changed in the Widget. Update it in settings.
	CurrentLocPath = NewPath;
	UELTEditorSettings::SetLocalizationPath(CurrentLocPath);

	// Update Localization directory name and path to CSV to the Widget.
	EditorWidget->FillLocalizationName(GetCurrentLocName());
	EditorWidget->FillCSVPath(PathsStringToList(GetCurrentCSVPath()));

	// Refresh available languages for this Localization directory and set them to the Widget.
	RefreshAvailableLangs(false);
	EditorWidget->FillAvailableLangsInLocFile(CurrentAvailableLangsForLocFile);

	// Set Global Namespace for this Localization directory to the Widget.
	EditorWidget->SetGlobalNamespace(GetCurrentGlobalNamespace());
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
	EditorWidget->FillCSVPath(PathsStringToList(GetCurrentCSVPath()));
}

void UELTEditor::OnGenerateLocFiles()
{
	// Generate Loc Files button has been pressed. 
	// Generate Loc Files and if succeeded refresh available languages and preview.
	FString ReturnMessage;
	if (GenerateLocFiles(ReturnMessage))
	{
		RefreshAvailableLangs(true);
		SetLanguagePreview();
	}
	
	// Display a Dialog Window to inform user that the localization generation has been finished.
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ReturnMessage));
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
		EditorWidget->SetSeparator(Separator);
	}
	else if (Separator.Len() > 1)
	{
		Separator = FString(1, *Separator);
		EditorWidget->SetSeparator(Separator);
	}
	UELTEditorSettings::SetSeparator(Separator);
}

void UELTEditor::OnLogDebugChanged(bool bNewLogDebug)
{
	// Log Debug flag has been changed in the Widget. Save this setting.
	UELTSettings::SetLogDebug(bNewLogDebug);
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
		EditorWidget->FillAvailableLangs(CurrentAvailableLangs);
		EditorWidget->FillAvailableLangsInLocFile(CurrentAvailableLangsForLocFile);
		
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
			EditorWidget->SetLocalizationPreviewLang(LangPreview);
		}

		if (CurrentAvailableLangs.Contains(LangAtFirstLaunch) == false)
		{
			UELTSettings::SetLanguageToOverrideAtFirstLaunch(TEXT(""));
		}
		else
		{
			EditorWidget->SetLocalizationOnFirstRunLang(LangAtFirstLaunch);
		}
	}

	// Set available languages to the game settings.
	UELTSettings::SetAvailableLanguages(CurrentAvailableLangs);
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
	const TArray<FString>& CSVFilePaths = PathsStringToList(GetCurrentCSVPath());
	const FString LocPath = FPaths::ConvertRelativePathToFull(CurrentLocPath);
	return GenerateLocFilesImpl(CSVFilePaths, LocPath, GetCurrentLocName(), GetCurrentGlobalNamespace(), UELTEditorSettings::GetSeparator(), OutMessage);
}

bool UELTEditor::GenerateLocFilesImpl(const FString& CSVPaths, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, const FString& Separator, FString& OutMessage)
{
	return GenerateLocFilesImpl(PathsStringToList(CSVPaths), LocPath, LocName, GlobalNamespace, Separator, OutMessage);
}

bool UELTEditor::GenerateLocFilesImpl(const TArray<FString>& CSVPaths, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, const FString& Separator, FString& OutMessage)
{
	if (Separator.Len() != 1)
	{
		OutMessage = FString::Printf(TEXT("ERROR: The Separator is invalid. Must be exactly 1 character. Current Separator = %s"), *Separator);
		return false;
	}

	const bool bLogDebug = UELTSettings::GetLogDebug();
	bool bFirstCSV = true;
	TMap<FString, FTextLocalizationResource> LocReses;
	for (const FString& CSVPath : CSVPaths)
	{
		const FString CSVFilePath = FPaths::ConvertRelativePathToFull(CSVPath);
		if (bLogDebug)
		{
			UE_LOG(ELTEditorLog, Log, TEXT("Parsing file: %s"), *CSVFilePath);
		}
		FCSVReader Reader;
		if (Reader.LoadFromFile(CSVFilePath, (*Separator)[0], OutMessage))
		{
			const TArray<FCSVColumn> Columns = Reader.Columns;

			if (Columns.Num() > 1)
			{
				const int32 NumOfValues = Columns[0].Values.Num();
				for (int32 CIdx = 1; CIdx < Columns.Num(); CIdx++)
				{
					if (Columns[CIdx].Values.Num() != NumOfValues)
					{
						OutMessage = FString::Printf(TEXT("ERROR: Invalid CSV! Column %i (counting from 1) has %i values while Column 1 has %i values. Every Column must have the same amount of values!"), CIdx+1, Columns[CIdx].Values.Num(), NumOfValues);
						return false;
					}
				}

				// Potential place for namespaces.
				const FCSVColumn& Namespaces = Columns[0];

				// Check if we have namespaces defined for every key or to use global value.
				const bool bHasNamespaces = Columns[0].Values[0].Equals(TEXT("namespace"), ESearchCase::IgnoreCase);
				const bool bUseGlobalNamespace = (bHasNamespaces == false) && (GlobalNamespace.IsEmpty() == false);

				if (bUseGlobalNamespace == false && bHasNamespaces == false)
				{
					OutMessage = TEXT("ERROR: Namespaces in CSV not found!");
					return false;
				}

				if (bFirstCSV)
				{
					IFileManager::Get().DeleteDirectory(*LocPath, false, true);
				}

				// Keys will be in first row if not having namespaces.
				const FCSVColumn& Keys = Columns[bHasNamespaces ? 1 : 0];

				if (bLogDebug)
				{
					UE_LOG(ELTEditorLog, Log, TEXT("Adding Entries"));
					UE_LOG(ELTEditorLog, Log, TEXT("[Lang] | [Namespace] | [Key] | [Value]"));
				}

				for (int32 Column = (bHasNamespaces ? 2 : 1); Column < Columns.Num(); Column++)
				{
					const FCSVColumn& Locs = Columns[Column];
					FString Lang = Locs.Values[0];
					if (Lang.RemoveFromStart(TEXT("lang-")))
					{
						Lang.ReplaceCharInline('_', '-');
						if (LocReses.Contains(Lang) == false)
						{
							LocReses.Add(Lang, FTextLocalizationResource());
						}
						FTextLocalizationResource& LocRes = LocReses[Lang];
						for (int32 Key = 1; Key < Keys.Values.Num(); Key++)
						{
							const FString& Namespace = (bUseGlobalNamespace || Namespaces.Values[Key].IsEmpty()) ? GlobalNamespace : Namespaces.Values[Key];
							if (Namespace.IsEmpty())
							{
								OutMessage = FString::Printf(TEXT("ERROR: Namespace in row %i (counting from 1) is empty!"), Key);
								return false;
							}

							if (bLogDebug)
							{
								UE_LOG(ELTEditorLog, Log, TEXT("%s | %s | %s | %s"), *Lang, *Namespace, *(Keys.Values[Key]), *(Locs.Values[Key]));
							}
							
							LocRes.AddEntry(
								FTextKey(Namespace),
								FTextKey(Keys.Values[Key]),
								Keys.Values[Key],
								Locs.Values[Key],
								0);
						}
					}
				}

				bFirstCSV = false;
			}
			else
			{
				OutMessage = TEXT("ERROR: CSV has not enough Columns!");
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	// LocMeta must be created for every localization path.
	FTextLocalizationMetaDataResource LocMeta;
	LocMeta.NativeCulture = TEXT("en");
	LocMeta.NativeLocRes = TEXT("en") / LocName + TEXT(".locres");
	const FString MetaFileName = LocPath / LocName + TEXT(".locmeta");
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