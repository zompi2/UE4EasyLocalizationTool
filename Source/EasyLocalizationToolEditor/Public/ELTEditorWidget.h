// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

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
DECLARE_DELEGATE_OneParam(FOnCSVPathChanged, const FString&);
DECLARE_DELEGATE(FOnGenerateLocFiles);
DECLARE_DELEGATE_OneParam(FOnReimportAtEditorStartupChanged, bool);
DECLARE_DELEGATE_OneParam(FOnLocalizationPreviewChanged, bool);
DECLARE_DELEGATE_OneParam(FOnLocalizationPreviewLangChanged, const FString&);
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
	UFUNCTION(BlueprintPure)
	FString GetPluginVersion();

	/**
	 * Call BP to fill up the list of localization paths.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void FillLocalizationPaths(const TArray<FString>& Paths);

	/**
	 * Call BP to display a current localization path.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationPath(const FString& Path);

	/**
	 * BP calls the code to inform that the localization path has been selected.
	 */
	UFUNCTION(BlueprintCallable)
	void OnLocalizationPathSelected(const FString& Path);

	/**
	 * Call BP to fill up a name of current localization path.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void FillLocalizationName(const FString& LocName);

	/**
	 * Call BP to fill up all available languages.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void FillAvailableLangs(const TArray<FString>& Langs);

	/**
	 * Call BP to fill up available languages in selected localization path.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void FillAvailableLangsInLocFile(const TArray<FString>& Langs);

	/**
	 * Call BP to fill up the path to the CSV file
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void FillCSVPath(const FString& CSVPath);


	/**
	 * User pressed "Select CSV" button.
	 */
	UFUNCTION(BlueprintCallable)
	void SelectNewCSVPath();

	/**
	 * User pressed "Generate" button.
	 */
	UFUNCTION(BlueprintCallable)
	void GenerateLocFiles();


	/**
	 * Set "Localization Preview" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationPreview(bool LocalizationPreview);

	/**
	 * "Localization Preview" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable)
	void OnLocalizationPreviewChanged(bool LocalizationPreview);



	/**
	 * Set "Localization Preview Language" to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationPreviewLang(const FString& PreviewLang);

	/**
	 * "Localization Preview Language" has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable)
	void OnLocalizationPreviewLangChanged(const FString& PreviewLang);



	/**
	 * Set "Reimport At Editor Startup" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetReimportAtEditorStartup(bool bReimportAtEditorStartup);

	/**
	 * "Reimport At Editor Startup" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable)
	void OnReimportAtEditorStartupChanged(bool bReimportAtEditorStartup);
	


	/**
	 * Set "Localization On First Run" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationOnFirstRun(bool LocalizationOnFirstRun);

	/**
	 * "Localization On First Run" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable)
	void OnLocalizationOnFirstRun(bool LocalizationOnFirstRun);



	/**
	 * Set "Localization On First Run Language" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationOnFirstRunLang(const FString& OnFirstRunLang);
	
	/**
	 * "Localization On First Run Language" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable)
	void OnLocalizationOnFirstRunLangChanged(const FString& OnFirstRunLang);



	/**
	 * Set "Global Namespace" option to the Widget.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetGlobalNamespace(const FString& GlobalNamespace);

	/**
	 * "Global Namespace" option has been changed on the Widget.
	 */
	UFUNCTION(BlueprintCallable)
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
	FOnLocalizationOnFirstRunChanged OnLocalizationOnFirstRunChangedDelegate;
	FOnLocalizationOnFirstRunLangChanged OnLocalizationOnFirstRunLangChangedDelegate;
	FOnGlobalNamespaceChanged OnGlobalNamespaceChangedDelegate;
};