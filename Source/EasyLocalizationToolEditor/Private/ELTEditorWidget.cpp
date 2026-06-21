// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "ELTEditorWidget.h"
#include "DesktopPlatformModule.h"
#include "Interfaces/IPluginManager.h"
#include "SELTEditorWidget.h"

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
	if (MyWidget.IsValid())
	{
		MyWidget->FillLocalizationPaths(Paths);
	}
}

void UELTEditorWidget::CallSetLocalizationPath(const FString& Path)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationPath(Path);
	}
}

void UELTEditorWidget::CallFillLocalizationName(const FString& LocName)
{
	if (MyWidget.IsValid())
	{
		MyWidget->FillLocalizationName(LocName);
	}
}

void UELTEditorWidget::CallFillAvailableLangs(const TArray<FString>& Langs)
{
	if (MyWidget.IsValid())
	{
		MyWidget->FillAvailableLangs(Langs);
	}
}

void UELTEditorWidget::CallFillAvailableLangsInLocFile(const TArray<FString>& Langs)
{
	if (MyWidget.IsValid())
	{
		MyWidget->FillAvailableLangsInLocFile(Langs);
	}
}

void UELTEditorWidget::CallFillCSVPath(const TArray<FString>& CSVPaths)
{
	if (MyWidget.IsValid())
	{
		MyWidget->FillCSVPath(CSVPaths);
	}
}

void UELTEditorWidget::CallSetLocalizationPreview(bool LocalizationPreview)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationPreview(LocalizationPreview);
	}
}

void UELTEditorWidget::CallSetLocalizationPreviewLang(const FString& PreviewLang)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationPreviewLang(PreviewLang);
	}
}

void UELTEditorWidget::CallSetReimportAtEditorStartup(bool bReimportAtEditorStartup)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetReimportAtEditorStartup(bReimportAtEditorStartup);
	}
}

void UELTEditorWidget::CallSetManuallySetLastUsedLanguage(bool bManuallySetLastUsedLanguage)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetManuallySetLastUsedLanguage(bManuallySetLastUsedLanguage);
	}
}

void UELTEditorWidget::CallSetLocalizationOnFirstRun(bool LocalizationOnFirstRun)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationOnFirstRun(LocalizationOnFirstRun);
	}
}

void UELTEditorWidget::CallSetGenerateKeyReferenceStringTable(bool bGenerateKeyReferenceStringTable)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetGenerateKeyReferenceStringTable(bGenerateKeyReferenceStringTable);
	}
}

void UELTEditorWidget::CallSetLocalizationOnFirstRunLang(const FString& OnFirstRunLang)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetLocalizationOnFirstRunLang(OnFirstRunLang);
	}
}

void UELTEditorWidget::CallSetGlobalNamespace(const FString& GlobalNamespace)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetGlobalNamespace(GlobalNamespace);
	}
}

void UELTEditorWidget::CallSetSeparator(const FString& Separator)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetSeparator(Separator);
	}
}

void UELTEditorWidget::CallSetFallbackWhenEmpty(const FString& FallbackWhenEmpty)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetFallbackWhenEmpty(FallbackWhenEmpty);
	}
}

void UELTEditorWidget::CallSetLogDebug(bool bLogDebug)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetLogDebug(bLogDebug);
	}
}

void UELTEditorWidget::CallSetPreviewInUI(bool bPreviewInUI)
{
	if (MyWidget.IsValid())
	{
		MyWidget->SetPreviewInUI(bPreviewInUI);
	}
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

void UELTEditorWidget::OnGenerateKeyReferenceStringTableChanged(bool bGenerateKeyReferenceStringTable)
{
	OnGenerateKeyReferenceStringTableChangedDelegate.ExecuteIfBound(bGenerateKeyReferenceStringTable);
}

void UELTEditorWidget::OnGlobalNamespaceChanged(const FString& NewGlobalNamespace)
{
	OnGlobalNamespaceChangedDelegate.ExecuteIfBound(NewGlobalNamespace);
}

void UELTEditorWidget::OnSeparatorChanged(const FString& NewGlobalNamespace)
{
	OnSeparatorChangedDelegate.ExecuteIfBound(NewGlobalNamespace);
}

void UELTEditorWidget::OnFallbackWhenEmptyChanged(const FString& NewFallback)
{
	OnFallbackWhenEmptyChangedDelegate.ExecuteIfBound(NewFallback);
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
	if (MyWidget.IsValid() == false)
	{
		MyWidget = SNew(SELTEditorWidget);
		MyWidget->WidgetController = this;
	}
	return MyWidget.ToSharedRef();
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION