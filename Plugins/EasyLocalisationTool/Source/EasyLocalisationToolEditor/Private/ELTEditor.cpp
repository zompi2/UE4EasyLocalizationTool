// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#include "ELTEditor.h"
#include "Internationalization/TextLocalizationResource.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "ELTEditorSettings.h"
#include "ELTEditorWidget.h"
#include "ELTSettings.h"

#include "AssetRegistryModule.h"
#include "EditorUtilityWidget.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"

#include "CSVReader.h"
#include "LevelEditor.h"

void UELTEditor::Init()
{
	CSVPaths		= UELTEditorSettings::GetCSVPaths();
	CurrentLocPath	= UELTEditorSettings::GetLocalisationPath();

	if (UELTEditorSettings::GetReimportAtEditorStartup())
	{
		FString OutMessage;
		GenerateLocFiles(OutMessage);
	}

	RefreshAvailableLangs(false);
	SetLanguagePreview();
}


UEditorUtilityWidgetBlueprint* UELTEditor::GetUtilityWidgetBlueprint()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath("/EasyLocalisationTool/ELTEditorWidget_BP.ELTEditorWidget_BP");
	return Cast<UEditorUtilityWidgetBlueprint>(AssetData.GetAsset());
}

bool UELTEditor::CanCreateEditorUI()
{
	return GetUtilityWidgetBlueprint() != nullptr;
}

TSharedRef<SWidget> UELTEditor::CreateEditorUI()
{
	FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditor.OnMapChanged().AddUObject(this, &UELTEditor::ChangeTabWorld);

	return CreateEditorWidget();
}

TSharedRef<SWidget> UELTEditor::CreateEditorWidget()
{
	TSharedRef<SWidget> CreatedWidget = SNullWidget::NullWidget;
	if (UEditorUtilityWidgetBlueprint* UtilityWidgetBP = GetUtilityWidgetBlueprint())
	{
		CreatedWidget = UtilityWidgetBP->CreateUtilityWidget();
		EditorWidget = Cast<UELTEditorWidget>(UtilityWidgetBP->GetCreatedWidget());
		InitializeTheWidget();
	}
	return CreatedWidget;
}

void UELTEditor::ChangeTabWorld(UWorld* World, EMapChangeType MapChangeType)
{
	if (MapChangeType == EMapChangeType::TearDownWorld)
	{
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
	else if (MapChangeType != EMapChangeType::SaveMap)
	{
		if (EditorTab.IsValid())
		{
			EditorTab.Pin()->SetContent(CreateEditorWidget());
		}
	}
}

void UELTEditor::InitializeTheWidget()
{
	// Check available languages (based on files in localisation directory)
	RefreshAvailableLangs(true);

	// Bind all required delegates
	EditorWidget->OnLocalisationPathSelectedDelegate.BindUObject(this, &UELTEditor::OnLocalisationPathChanged);
	EditorWidget->OnCSVPathChangedDelegate.BindUObject(this, &UELTEditor::OnCSVPathChanged);
	EditorWidget->OnGenerateLocFilesDelegate.BindUObject(this, &UELTEditor::OnGenerateLocFiles);
	EditorWidget->OnReimportAtEditorStartupChangedDelegate.BindUObject(this, &UELTEditor::OnReimportAtEditorStartupChanged);
	EditorWidget->OnLocalisationPreviewChangedDelegate.BindUObject(this, &UELTEditor::OnLocalisationPreviewChanged);
	EditorWidget->OnLocalisationPreviewLangChangedDelegate.BindUObject(this, &UELTEditor::OnLocalisationPreviewLangChanged);
	EditorWidget->OnLocalisationOnFirstRunChangedDelegate.BindUObject(this, &UELTEditor::OnLocalisationFirstRunChanged);
	EditorWidget->OnLocalisationOnFirstRunLangChangedDelegate.BindUObject(this, &UELTEditor::OnLocalisationFirstRunLangChanged);
	EditorWidget->OnGlobalNamespaceChangedDelegate.BindUObject(this, &UELTEditor::OnGlobalNamespaceChanged);

	// Fill localisation paths list on the widget
	const TArray<FString>& GameLocPaths = FPaths::GetGameLocalizationPaths();
	EditorWidget->FillLocalisationPaths(GameLocPaths);
	if (GameLocPaths.Num() > 0)
	{
		if (GameLocPaths.Contains(CurrentLocPath))
		{
			EditorWidget->SetLocalisationPath(CurrentLocPath);
		}
		else
		{
			EditorWidget->SetLocalisationPath(GameLocPaths[0]);
		}
	}

	// Load current value of reimport on editor startup option
	EditorWidget->OnSetReimportAtEditorStartupChanged(UELTEditorSettings::GetReimportAtEditorStartup());

	// Load current value of localisaiton preview option
	EditorWidget->SetLocalisationPreview(UELTEditorSettings::GetLocalisationPreview());
	EditorWidget->SetLocalisationPreviewLang(UELTEditorSettings::GetLocalisationPreviewLang());

	// Load current value of localisaiton override language at first run option
	EditorWidget->SetLocalisationOnFirstRun(UELTSettings::GetOverrideLanguageAtFirstLaunch());
	EditorWidget->SetLocalisationOnFirstRunLang(UELTSettings::GetLanguageToOverrideAtFirstLaunch());

	// Load current namespace
	const TMap<FString, FString>& GlobalNamespaces = UELTEditorSettings::GetGlobalNamespaces();
	if (GlobalNamespaces.Contains(GetCurrentLocName()))
	{
		EditorWidget->SetGlobalNamespace(GlobalNamespaces[GetCurrentLocName()]);
	}
	else
	{
		EditorWidget->SetGlobalNamespace(TEXT(""));
	}
}



void UELTEditor::OnLocalisationPathChanged(const FString& NewPath)
{
	CurrentLocPath = NewPath;
	UELTEditorSettings::SetLocalisationPath(CurrentLocPath);

	EditorWidget->FillLocalisationName(GetCurrentLocName());
	EditorWidget->FillCSVPath(GetCurrentCSVPath());

	RefreshAvailableLangs(false);
	EditorWidget->FillAvailableLangsInLocFile(CurrentAvailableLangsForLocFile);

	EditorWidget->SetGlobalNamespace(GetCurrentGlobalNamespace());
}

void UELTEditor::OnCSVPathChanged(const FString& NewPath)
{
	if (CSVPaths.Contains(GetCurrentLocName()))
	{
		CSVPaths[GetCurrentLocName()] = NewPath;
	}
	else
	{
		CSVPaths.Add(GetCurrentLocName(), NewPath);
	}
	UELTEditorSettings::SetCSVPaths(CSVPaths);
	EditorWidget->FillCSVPath(GetCurrentCSVPath());
}

void UELTEditor::OnGenerateLocFiles()
{
	FString ReturnMessage;
	if (GenerateLocFiles(ReturnMessage))
	{
		RefreshAvailableLangs(true);
		FTextLocalizationManager::Get().RefreshResources();
		SetLanguagePreview();
	}
	
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ReturnMessage));
}

void UELTEditor::OnReimportAtEditorStartupChanged(bool bNewReimportAtEditorStartup)
{
	UELTEditorSettings::SetReimportAtEditorStartup(bNewReimportAtEditorStartup);
}

void UELTEditor::OnLocalisationPreviewChanged(bool bNewLocalisationPreview)
{
	UELTEditorSettings::SetLocalisationPreview(bNewLocalisationPreview);
	SetLanguagePreview();
}

void UELTEditor::OnLocalisationPreviewLangChanged(const FString& LangPreview)
{
	UELTEditorSettings::SetLocalisationPreiewLang(LangPreview);
	SetLanguagePreview();
}

void UELTEditor::OnLocalisationFirstRunChanged(bool bOnFirstRun)
{
	UELTSettings::SetOverrideLanguageAtFirstLaunch(bOnFirstRun);
}

void UELTEditor::OnLocalisationFirstRunLangChanged(const FString& LangOnFirstRun)
{
	UELTSettings::SetLanguageToOverrideAtFirstLaunch(LangOnFirstRun);
}

void UELTEditor::OnGlobalNamespaceChanged(const FString& NewGlobalNamespace)
{
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


void UELTEditor::SetLanguagePreview()
{
	FTextLocalizationManager::Get().DisableGameLocalizationPreview();
	const FString& CurrentLang = UELTEditorSettings::GetLocalisationPreviewLang();
	if (UELTEditorSettings::GetLocalisationPreview() && CurrentAvailableLangs.Contains(CurrentLang))
	{
		FTextLocalizationManager::Get().EnableGameLocalizationPreview(CurrentLang);
	}
}

void UELTEditor::RefreshAvailableLangs(bool bRefreshUI)
{
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
		FString LangPreview = UELTEditorSettings::GetLocalisationPreviewLang();
		FString LangAtFirstLaunch = UELTSettings::GetLanguageToOverrideAtFirstLaunch();
		EditorWidget->FillAvailableLangs(CurrentAvailableLangs);
		EditorWidget->FillAvailableLangsInLocFile(CurrentAvailableLangsForLocFile);

		if (CurrentAvailableLangs.Contains(LangPreview) == false)
		{
			UELTEditorSettings::SetLocalisationPreiewLang(TEXT(""));
		}
		else
		{
			EditorWidget->SetLocalisationPreviewLang(LangPreview);
		}

		if (CurrentAvailableLangs.Contains(LangAtFirstLaunch) == false)
		{
			UELTSettings::SetLanguageToOverrideAtFirstLaunch(TEXT(""));
		}
		else
		{
			EditorWidget->SetLocalisationOnFirstRunLang(LangAtFirstLaunch);
		}
	}

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
	const FString CSVFilePath = FPaths::ConvertRelativePathToFull(GetCurrentCSVPath());
	const FString LocPath = FPaths::ConvertRelativePathToFull(CurrentLocPath);
	return GenerateLocFilesImpl(CSVFilePath, LocPath, GetCurrentLocName(), GetCurrentGlobalNamespace(), OutMessage);
}

bool UELTEditor::GenerateLocFilesImpl(const FString& CSVPath, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, FString& OutMessage)
{
	const FString CSVFilePath = FPaths::ConvertRelativePathToFull(CSVPath);
	FCSVReader Reader;
	if (Reader.LoadFromFile(CSVFilePath, OutMessage))
	{
		const TArray<FCSVColumn> Columns = Reader.Columns;

		if (Columns.Num() > 1)
		{
			// Potential place for namespaces
			const FCSVColumn& Namespaces = Columns[0];

			// Check if we have namespaces defined for every key or to use global value
			const bool bHasNamespaces = Columns[0].Values[0].Equals(TEXT("namespace"), ESearchCase::IgnoreCase);
			const bool bUseGlobalNamespace = (bHasNamespaces == false) && (GlobalNamespace.IsEmpty() == false);

			if (bUseGlobalNamespace == false && bHasNamespaces == false)
			{
				OutMessage = TEXT("ERROR: Namespaces in csv not found!");
				return false;
			}

			IFileManager::Get().DeleteDirectory(*LocPath, false, true);

			// Keys will be in first row if not having namespaces
			const FCSVColumn& Keys = Columns[bHasNamespaces ? 1 : 0];

			for (int32 Column = (bHasNamespaces ? 2 : 1); Column < Columns.Num(); Column++)
			{
				const FCSVColumn& Locs = Columns[Column];
				FString Lang = Locs.Values[0];
				if (Lang.RemoveFromStart(TEXT("lang-")))
				{
					FTextLocalizationResource LocRes;
					for (int32 Key = 1; Key < Keys.Values.Num(); Key++)
					{
						const FString& Namespace = (bUseGlobalNamespace || Namespaces.Values[Key].IsEmpty()) ? GlobalNamespace : Namespaces.Values[Key];
						if (Namespace.IsEmpty())
						{
							OutMessage = TEXT("ERROR: Namespace is empty!");
							return false;
						}
						LocRes.AddEntry(
							FTextKey(Namespace),
							FTextKey(Keys.Values[Key]),
							Keys.Values[Key],
							Locs.Values[Key],
							0);
					}
					LocRes.SaveToFile(LocPath / Lang / LocName + TEXT(".locres"));
				}
			}
			FTextLocalizationMetaDataResource LocMeta;
			LocMeta.NativeCulture = TEXT("en");
			LocMeta.NativeLocRes = TEXT("en") / LocName + TEXT(".locres");
			LocMeta.SaveToFile(LocPath / LocName + TEXT(".locmeta"));
		}
		else
		{
			OutMessage = TEXT("ERROR: Not enought rows in csv!");
			return false;
		}
	}
	else
	{
		return false;
	}

	OutMessage = TEXT("SUCCESS: Loc import complete!");
	return true;
}

const FString& UELTEditor::GetCurrentCSVPath()
{
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

const FString UELTEditor::GetCurrentLocName()
{
	if (CurrentLocPath.IsEmpty() == false)
	{
		return FPaths::GetBaseFilename(CurrentLocPath);
	}
	else
	{
		static FString Dummy = TEXT("");
		return Dummy;
	}
}

const FString UELTEditor::GetCurrentGlobalNamespace()
{
	const TMap<FString, FString>& GlobalNamespaces = UELTEditorSettings::GetGlobalNamespaces();
	if (GlobalNamespaces.Contains(GetCurrentLocName()))
	{
		return GlobalNamespaces[GetCurrentLocName()];
	}
	else
	{
		static FString Dummy = TEXT("");
		return Dummy;
	}
}