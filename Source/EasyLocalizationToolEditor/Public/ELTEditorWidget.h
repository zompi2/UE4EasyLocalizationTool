// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

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

	/**
	 * Call BP to display a current localization path.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void SetLocalizationPath(const FString& Path);

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

	/**
	 * Call BP to fill up all available languages.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void FillAvailableLangs(const TArray<FString>& Langs);

	/**
	 * Call BP to fill up available languages in selected localization path.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void FillAvailableLangsInLocFile(const TArray<FString>& Langs);

	/**
	 * Call BP to fill up the path to the CSV file
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Easy Localization Tool Editor")
	void FillCSVPath(const FString& CSVPath);


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

	/**
	 * "Global Namespace" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool Editor")
	void OnGlobalNamespaceChanged(const FString& NewGlobalNamespace);



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
};