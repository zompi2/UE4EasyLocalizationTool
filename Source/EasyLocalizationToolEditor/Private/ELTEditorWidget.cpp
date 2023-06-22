// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#include "ELTEditorWidget.h"
#include "DesktopPlatformModule.h"
#include "Interfaces/IPluginManager.h"

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

void UELTEditorWidget::OnLocalizationPathSelected(const FString& Path)
{
	OnLocalizationPathSelectedDelegate.ExecuteIfBound(Path);
}

void UELTEditorWidget::SelectNewCSVPath()
{
	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		TArray<FString> OutFileNames;
		if (DesktopPlatform->OpenFileDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr), TEXT("Pick CSV"), FPaths::ProjectDir(), TEXT(""), TEXT("CSV|*.csv"), EFileDialogFlags::None, OutFileNames))
		{
			if (OutFileNames.Num() > 0)
			{
				OnCSVPathChangedDelegate.ExecuteIfBound(OutFileNames[0]);
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

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION