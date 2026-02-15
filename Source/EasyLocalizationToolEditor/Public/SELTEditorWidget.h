// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SUserWidget.h"

class UELTEditorWidget;

class EASYLOCALIZATIONTOOLEDITOR_API SELTEditorWidget : public SUserWidget
{

public:

	SLATE_BEGIN_ARGS(SELTEditorWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	static TSharedRef<SELTEditorWidget> New();

	void FillLocalizationPaths(const TArray<FString>& Paths);
	void SetLocalizationPath(const FString& Path);
	void FillLocalizationName(const FString& LocName);
	void FillAvailableLangs(const TArray<FString>& Langs);
	void FillAvailableLangsInLocFile(const TArray<FString>& Langs);
	void FillCSVPath(const TArray<FString>& CSVPaths);
	void SetLocalizationPreview(bool bLocalizationPreview);
	void SetLocalizationPreviewLang(const FString& PreviewLang);
	void SetReimportAtEditorStartup(bool bReimportAtEditorStartup);
	void SetManuallySetLastUsedLanguage(bool bManuallySetLastUsedLanguage);
	void SetLocalizationOnFirstRun(bool LocalizationOnFirstRun);
	void SetLocalizationOnFirstRunLang(const FString& OnFirstRunLang);
	void SetGlobalNamespace(const FString& GlobalNamespace);
	void SetSeparator(const FString& Separator);
	void SetLogDebug(bool bLogDebug);
	void SetPreviewInUI(bool bPreviewInUI);

	TWeakObjectPtr<UELTEditorWidget> WidgetController = nullptr;

	TArray<TSharedPtr<FString>> LocPathsList;
	TSharedPtr<FString> SelectedLocPath = nullptr;

	TArray<TSharedPtr<FString>> PreviewsAvailables;
	TSharedPtr<FString> SelectedPreviewLang = nullptr;

	TArray<TSharedPtr<FString>> LanguageOverridesAvailable;
	TSharedPtr<FString> SelectedLanguageOverride = nullptr;

	bool bReimportAtEditorStartup_Chkbox = false;
	bool bIsLocalisationPreviewEnabled_Chkbox = false;
	bool bManuallySetLastLanguage_Chkbox = false;
	bool bOverrideLanguageOnStartup_Chkbox = false;
	bool bLogDebug_Chkbox = false;
	bool bPreviewInUI_Chkbox = false;

	FString CurrentLocName;
	FString AvailableLangs;
	FString AvailableLangsInLocFile;
	FString SeparatorValue;
	FString CSVFiles;
	FString GlobalNamespaceValue;
};