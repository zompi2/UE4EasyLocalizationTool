// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#include "ELTEditorWidget.h"
#include "DesktopPlatformModule.h"

void UELTEditorWidget::OnLocalisationPathSelected(const FString& Path)
{
	OnLocalisationPathSelectedDelegate.ExecuteIfBound(Path);
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

void UELTEditorWidget::SetReimportAtEditorStartup(bool bReimportAtEditorStartup)
{
	OnReimportAtEditorStartupChangedDelegate.ExecuteIfBound(bReimportAtEditorStartup);
}

void UELTEditorWidget::OnLocalisationPreviewChanged(bool LocalisationPreview)
{
	OnLocalisationPreviewChangedDelegate.ExecuteIfBound(LocalisationPreview);
}

void UELTEditorWidget::OnLocalisationPreviewLangChanged(const FString& NewPreviewLang)
{
	OnLocalisationPreviewLangChangedDelegate.ExecuteIfBound(NewPreviewLang);
}

void UELTEditorWidget::OnLocalisationOnFirstRun(bool LocalisationOnFirstRun)
{
	OnLocalisationOnFirstRunChangedDelegate.ExecuteIfBound(LocalisationOnFirstRun);
}

void UELTEditorWidget::OnLocalisationOnFirstRunLangChanged(const FString& OnFirstRunLang)
{
	OnLocalisationOnFirstRunLangChangedDelegate.ExecuteIfBound(OnFirstRunLang);
}

void UELTEditorWidget::OnGlobalNamespaceChanged(const FString& NewGlobalNamespace)
{
	OnGlobalNamespaceChangedDelegate.ExecuteIfBound(NewGlobalNamespace);
}