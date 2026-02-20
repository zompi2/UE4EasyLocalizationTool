// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "ELTEditorWidget.generated.h"

/**
 * Widget code used to communicate between code and BP widget.
 */

/**
 * Declaration of all required delegates for a proper communication with UI.
 * They will be broadcasted to inform the plugin code about changes in UI.
 */
DECLARE_DELEGATE_OneParam(FOnLocalizationPathSelected, const FString&);
DECLARE_DELEGATE_OneParam(FOnCSVPathChanged, const TArray<FString>&);
DECLARE_DELEGATE(FOnGenerateLocFiles);
DECLARE_DELEGATE_OneParam(FOnReimportAtEditorStartupChanged, bool);
DECLARE_DELEGATE_OneParam(FOnLocalizationPreviewChanged, bool);
DECLARE_DELEGATE_OneParam(FOnLocalizationPreviewLangChanged, const FString&);
DECLARE_DELEGATE_OneParam(FManuallySetLastLanguageChanged, bool);
DECLARE_DELEGATE_OneParam(FOnLocalizationOnFirstRunChanged, bool);
DECLARE_DELEGATE_OneParam(FOnLocalizationOnFirstRunLangChanged, const FString&);
DECLARE_DELEGATE_OneParam(FOnGlobalNamespaceChanged, const FString&);
DECLARE_DELEGATE_OneParam(FOnSeparatorChanged, const FString&);
DECLARE_DELEGATE_OneParam(FOnLogDebugChanged, bool);
DECLARE_DELEGATE_OneParam(FOnPreviewInUIChanged, bool);

class SELTEditorWidget;

UCLASS()
class EASYLOCALIZATIONTOOLEDITOR_API UELTEditorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	/**
	 * Returns a version of the plugin.
	 */
	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool Editor")
	FString GetPluginVersion();

	/**
	 * Call BP to fill up the list of localization paths.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void FillLocalizationPaths(const TArray<FString>& Paths);
	void CallFillLocalizationPaths(const TArray<FString>& Paths);

	/**
	 * Call BP to display a current localization path.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetLocalizationPath(const FString& Path);
	void CallSetLocalizationPath(const FString& Path);

	/**
	 * BP calls the code to inform that the localization path has been selected.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnLocalizationPathSelected(const FString& Path);

	/**
	 * Call BP to fill up a name of current localization path.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void FillLocalizationName(const FString& LocName);
	void CallFillLocalizationName(const FString& LocName);

	/**
	 * Call BP to fill up all available languages.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void FillAvailableLangs(const TArray<FString>& Langs);
	void CallFillAvailableLangs(const TArray<FString>& Langs);

	/**
	 * Call BP to fill up available languages in selected localization path.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void FillAvailableLangsInLocFile(const TArray<FString>& Langs);
	void CallFillAvailableLangsInLocFile(const TArray<FString>& Langs);

	/**
	 * Call BP to fill up the path to the CSV file
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void FillCSVPath(const TArray<FString>& CSVPaths);
	void CallFillCSVPath(const TArray<FString>& CSVPaths);


	/**
	 * User pressed "Select CSV" button.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void SelectNewCSVPath();

	/**
	 * User pressed "Generate" button.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void GenerateLocFiles();


	/**
	 * Set "Localization Preview" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetLocalizationPreview(bool LocalizationPreview);
	void CallSetLocalizationPreview(bool LocalizationPreview);

	/**
	 * "Localization Preview" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor Editor")
	void OnLocalizationPreviewChanged(bool LocalizationPreview);



	/**
	 * Set "Localization Preview Language" to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetLocalizationPreviewLang(const FString& PreviewLang);
	void CallSetLocalizationPreviewLang(const FString& PreviewLang);

	/**
	 * "Localization Preview Language" has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnLocalizationPreviewLangChanged(const FString& PreviewLang);



	/**
	 * Set "Reimport At Editor Startup" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetReimportAtEditorStartup(bool bReimportAtEditorStartup);
	void CallSetReimportAtEditorStartup(bool bReimportAtEditorStartup);

	/**
	 * "Reimport At Editor Startup" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnReimportAtEditorStartupChanged(bool bReimportAtEditorStartup);
	


	/**
	 * Set "Manually Set Last Used Language" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetManuallySetLastUsedLanguage(bool bManuallySetLastUsedLanguage);
	void CallSetManuallySetLastUsedLanguage(bool bManuallySetLastUsedLanguage);

	/**
	 * "Manually Set Last Used Language" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnManuallySetLastUsedLanguageChanged(bool bManuallySetLastUsedLanguage);



	/**
	 * Set "Localization On First Run" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetLocalizationOnFirstRun(bool LocalizationOnFirstRun);
	void CallSetLocalizationOnFirstRun(bool LocalizationOnFirstRun);

	/**
	 * "Localization On First Run" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnLocalizationOnFirstRun(bool LocalizationOnFirstRun);



	/**
	 * Set "Localization On First Run Language" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetLocalizationOnFirstRunLang(const FString& OnFirstRunLang);
	void CallSetLocalizationOnFirstRunLang(const FString& OnFirstRunLang);
	
	/**
	 * "Localization On First Run Language" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnLocalizationOnFirstRunLangChanged(const FString& OnFirstRunLang);



	/**
	 * Set "Global Namespace" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetGlobalNamespace(const FString& GlobalNamespace);
	void CallSetGlobalNamespace(const FString& GlobalNamespace);

	/**
	 * "Global Namespace" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnGlobalNamespaceChanged(const FString& NewGlobalNamespace);



	/**
	 * Set "Separator" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetSeparator(const FString& Separator);
	void CallSetSeparator(const FString& Separator);

	/**
	 * "Separator" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnSeparatorChanged(const FString& NewSeparator);

	void CallSetFallbackWhenEmpty(const FString& FallbackWhenEmpty);

	/**
	 * Set "Log Debug" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetLogDebug(bool bLogDebug);
	void CallSetLogDebug(bool bLogDebug);

	/**
	 * "Log Debug" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnLogDebugChanged(bool bNewLogDebug);



	/**
	 * Set "Log Debug" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetPreviewInUI(bool bPreviewInUI);
	void CallSetPreviewInUI(bool bPreviewInUI);

	/**
	 * "Log Debug" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnPreviewInUIChanged(bool bNewPreviewInUI);

	/**
	 * Returns true if the "Preview In UI" option is supported.
	 * If it's not supported - do not display it in the Widget.
	 */
	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool Editor")
	bool IsPreviewInUISupported();


	/**
	 * Callbacks to be binded to the ELTEditor methods.
	 */
	FOnLocalizationPathSelected OnLocalizationPathSelectedDelegate;
	FOnCSVPathChanged OnCSVPathChangedDelegate;
	FOnGenerateLocFiles OnGenerateLocFilesDelegate;
	FOnReimportAtEditorStartupChanged OnReimportAtEditorStartupChangedDelegate;
	FOnLocalizationPreviewChanged OnLocalizationPreviewChangedDelegate;
	FOnLocalizationPreviewLangChanged OnLocalizationPreviewLangChangedDelegate;
	FManuallySetLastLanguageChanged OnManuallySetLastLanguageChangedDelegate;
	FOnLocalizationOnFirstRunChanged OnLocalizationOnFirstRunChangedDelegate;
	FOnLocalizationOnFirstRunLangChanged OnLocalizationOnFirstRunLangChangedDelegate;
	FOnGlobalNamespaceChanged OnGlobalNamespaceChangedDelegate;
	FOnSeparatorChanged OnSeparatorChangedDelegate;
	FOnLogDebugChanged OnLogDebugChangedDelegate;
	FOnPreviewInUIChanged OnPreviewInUIChangedDelegate;

	TSharedPtr<SELTEditorWidget> MyWidget = nullptr;
	TSharedRef<SWidget> GetWidget();
};