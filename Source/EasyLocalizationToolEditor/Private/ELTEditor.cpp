// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "ELTEditor.h"
#include "ELTImporter.h"
#include "Internationalization/TextLocalizationManager.h"

#include "ELTEngineVersionComparsion.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "ELTEditorSettings.h"
#include "ELTEditorWidget.h"
#include "ELTSettings.h"

#if UE_VERSION_NEWER_THAN_OR_EQUAL(5,1,0)
#include "AssetRegistry/AssetRegistryModule.h"
#else
#include "AssetRegistryModule.h"
#endif

#include "EditorUtilityWidget.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "LevelEditor.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

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
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5,1,0)
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath("/EasyLocalizationTool/ELTEditorWidget_BP.ELTEditorWidget_BP"));
#else
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath("/EasyLocalizationTool/ELTEditorWidget_BP.ELTEditorWidget_BP");
#endif
	return Cast<UEditorUtilityWidgetBlueprint>(AssetData.GetAsset());
}

bool UELTEditor::CanCreateEditorUI()
{
	return true;
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

	EditorWidget = NewObject<UELTEditorWidget>(this);
	if (EditorWidget)
	{
		CreatedWidget = EditorWidget->GetWidget();
		InitializeTheWidget();
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
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5,3,0)
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
#if UE_VERSION_NEWER_THAN_OR_EQUAL(5,8,0)
		FTextLocalizationManager::Get().ConfigureGameLocalizationPreviewLanguage(CurrentLang);
#endif
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

// IMPORTANT.
// CSV must have the following structure:
// |Namespace|DevNotes|Key|lang-en|lang-pl|...|
// The order of Namespace, DevNotes and Key columns can be different, but they must be before any lang-... column.
// DevNotes column is optional.
// If there is no Namespace column - the global namespace will be used.
// Everything with lang-... will be treated as a language
// Any other key/value will be ignored
// Any column before Namespace, DevNotes or Key column will be ignored.
bool UELTEditor::GenerateLocFiles(FString& OutMessage)
{
	if (CurrentLocPath.IsEmpty() || GetCurrentLocName().IsEmpty())
	{
		OutMessage = TEXT("The Localization Name or Path is Empty! Can't generate loc files.\nEnsure the [Internationalization] section in DefaultGame.ini has at least one LocalizationPath.");
		return false;
	}
	const TArray<FString>& CSVFilePaths = PathsStringToList(GetCurrentCSVPath());
	const FString LocPath = FPaths::ConvertRelativePathToFull(CurrentLocPath);

	return FELTImporter::GenerateLoc(
		CSVFilePaths, 
		LocPath, 
		GetCurrentLocName(),
		GetCurrentGlobalNamespace(),
		UELTEditorSettings::GetSeparator(),
		FELTImporter::FallbackStringToEnum(UELTEditorSettings::GetFallbackWhenEmpty()),
		UELTEditorSettings::GetGenerateKeyReferenceStringTable(),
		true, // Save to files.
		false, // Don't cache LocReses in memory. We don't need them in Editor.
		UELTEditorSettings::GetLogDebug(),
		OutMessage);
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