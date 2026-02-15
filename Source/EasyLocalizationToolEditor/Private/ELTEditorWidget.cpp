// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "ELTEditorWidget.h"
#include "DesktopPlatformModule.h"
#include "Interfaces/IPluginManager.h"

#if ELTEDITOR_USE_SLATE_EDITOR_UI
#include "SELTEditorWidget.h"
#endif

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

FString UELTEditorWidget::GetPluginVersion()
{
	TArray<TSharedRef<IPlugin>> Plugins = IPluginManager::Get().GetDiscoveredPlugins();
	for (const TSharedRef<IPlugin>& Plugin : Plugins) {
		if (Plugin->GetName() == "EasyLocalizationTool") {
			return Plugin->GetDescriptor().VersionName;
		}
	}
	return TEXT("");
}

void UELTEditorWidget::CallFillLocalizationPaths(const TArray<FString>& Paths)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->FillLocalizationPaths(Paths);
	}
#else
	FillLocalizationPaths(Paths);
#endif
}

void UELTEditorWidget::CallSetLocalizationPath(const FString& Path)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationPath(Path);
	}
#else
	SetLocalizationPath(Path);
#endif
}

void UELTEditorWidget::CallFillLocalizationName(const FString& LocName)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->FillLocalizationName(LocName);
	}
#else
	FillLocalizationName(LocName);
#endif
}

void UELTEditorWidget::CallFillAvailableLangs(const TArray<FString>& Langs)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->FillAvailableLangs(Langs);
	}
#else
	FillAvailableLangs(Langs);
#endif
}

void UELTEditorWidget::CallFillAvailableLangsInLocFile(const TArray<FString>& Langs)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->FillAvailableLangsInLocFile(Langs);
	}
#else
	FillAvailableLangsInLocFile(Langs);
#endif
}

void UELTEditorWidget::CallFillCSVPath(const TArray<FString>& CSVPaths)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->FillCSVPath(CSVPaths);
	}
#else
	FillCSVPath(CSVPaths);
#endif
}

void UELTEditorWidget::CallSetLocalizationPreview(bool LocalizationPreview)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationPreview(LocalizationPreview);
	}
#else
	SetLocalizationPreview(LocalizationPreview);
#endif
}

void UELTEditorWidget::CallSetLocalizationPreviewLang(const FString& PreviewLang)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationPreviewLang(PreviewLang);
	}
#else
	SetLocalizationPreviewLang(PreviewLang);
#endif
}

void UELTEditorWidget::CallSetReimportAtEditorStartup(bool bReimportAtEditorStartup)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetReimportAtEditorStartup(bReimportAtEditorStartup);
	}
#else
	SetReimportAtEditorStartup(bReimportAtEditorStartup);
#endif
}

void UELTEditorWidget::CallSetManuallySetLastUsedLanguage(bool bManuallySetLastUsedLanguage)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetManuallySetLastUsedLanguage(bManuallySetLastUsedLanguage);
	}
#else
	SetManuallySetLastUsedLanguage(bManuallySetLastUsedLanguage);
#endif
}

void UELTEditorWidget::CallSetLocalizationOnFirstRun(bool LocalizationOnFirstRun)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationOnFirstRun(LocalizationOnFirstRun);
	}
#else
	SetLocalizationOnFirstRun(LocalizationOnFirstRun);
#endif
}

void UELTEditorWidget::CallSetLocalizationOnFirstRunLang(const FString& OnFirstRunLang)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationOnFirstRunLang(OnFirstRunLang);
	}
#else
	SetLocalizationOnFirstRunLang(OnFirstRunLang);
#endif
}

void UELTEditorWidget::CallSetGlobalNamespace(const FString& GlobalNamespace)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetGlobalNamespace(GlobalNamespace);
	}
#else
	SetGlobalNamespace(GlobalNamespace);
#endif
}

void UELTEditorWidget::CallSetSeparator(const FString& Separator)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetSeparator(Separator);
	}
#else
	SetSeparator(Separator);
#endif
}

void UELTEditorWidget::CallSetLogDebug(bool bLogDebug)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetLogDebug(bLogDebug);
	}
#else
	SetLogDebug(bLogDebug);
#endif
}

void UELTEditorWidget::CallSetPreviewInUI(bool bPreviewInUI)
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid())
	{
		MyWidget->SetPreviewInUI(bPreviewInUI);
	}
#else
	SetPreviewInUI(bPreviewInUI);
#endif
}


void UELTEditorWidget::OnLocalizationPathSelected(const FString& Path)
{
	OnLocalizationPathSelectedDelegate.ExecuteIfBound(Path);
}

void UELTEditorWidget::SelectNewCSVPath()
{
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		TArray<FString> OutFileNames;
		if (DesktopPlatform->OpenFileDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr), TEXT("Pick CSV"), FPaths::ProjectDir(), TEXT(""), TEXT("CSV|*.csv"), EFileDialogFlags::Multiple, OutFileNames))
		{
			if (OutFileNames.Num() > 0)
			{
				OnCSVPathChangedDelegate.ExecuteIfBound(OutFileNames);
			}
		}
	}
}

void UELTEditorWidget::GenerateLocFiles()
{
	OnGenerateLocFilesDelegate.ExecuteIfBound();
}

void UELTEditorWidget::OnReimportAtEditorStartupChanged(bool bReimportAtEditorStartup)
{
	OnReimportAtEditorStartupChangedDelegate.ExecuteIfBound(bReimportAtEditorStartup);
}

void UELTEditorWidget::OnLocalizationPreviewChanged(bool LocalizationPreview)
{
	OnLocalizationPreviewChangedDelegate.ExecuteIfBound(LocalizationPreview);
}

void UELTEditorWidget::OnLocalizationPreviewLangChanged(const FString& NewPreviewLang)
{
	OnLocalizationPreviewLangChangedDelegate.ExecuteIfBound(NewPreviewLang);
}

void UELTEditorWidget::OnManuallySetLastUsedLanguageChanged(bool bManuallySetLastUsedLanguage)
{
	OnManuallySetLastLanguageChangedDelegate.ExecuteIfBound(bManuallySetLastUsedLanguage);
}

void UELTEditorWidget::OnLocalizationOnFirstRun(bool LocalizationOnFirstRun)
{
	OnLocalizationOnFirstRunChangedDelegate.ExecuteIfBound(LocalizationOnFirstRun);
}

void UELTEditorWidget::OnLocalizationOnFirstRunLangChanged(const FString& OnFirstRunLang)
{
	OnLocalizationOnFirstRunLangChangedDelegate.ExecuteIfBound(OnFirstRunLang);
}

void UELTEditorWidget::OnGlobalNamespaceChanged(const FString& NewGlobalNamespace)
{
	OnGlobalNamespaceChangedDelegate.ExecuteIfBound(NewGlobalNamespace);
}

void UELTEditorWidget::OnSeparatorChanged(const FString& NewGlobalNamespace)
{
	OnSeparatorChangedDelegate.ExecuteIfBound(NewGlobalNamespace);
}

void UELTEditorWidget::OnLogDebugChanged(bool bNewLogDebug)
{
	OnLogDebugChangedDelegate.ExecuteIfBound(bNewLogDebug);
}

void UELTEditorWidget::OnPreviewInUIChanged(bool bNewPreviewInUI)
{
	OnPreviewInUIChangedDelegate.ExecuteIfBound(bNewPreviewInUI);
}

bool UELTEditorWidget::IsPreviewInUISupported()
{
#if ELTEDITOR_WITH_PREVIEW_IN_UI
	return true;
#else
	return false;	
#endif
}

TSharedRef<SWidget> UELTEditorWidget::GetWidget()
{
#if ELTEDITOR_USE_SLATE_EDITOR_UI
	if (MyWidget.IsValid() == false)
	{
		MyWidget = SNew(SELTEditorWidget);
		MyWidget->WidgetController = this;
	}
	return MyWidget.ToSharedRef();
#else
	return SNullWidget::NullWidget;
#endif
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION