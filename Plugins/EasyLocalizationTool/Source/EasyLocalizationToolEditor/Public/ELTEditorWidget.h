// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blutility/Classes/EditorUtilityWidget.h"
#include "ELTEditorWidget.generated.h"

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

	UFUNCTION(BlueprintImplementableEvent)
	void FillLocalizationPaths(const TArray<FString>& Paths);

	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationPath(const FString& Path);

	UFUNCTION(BlueprintCallable)
	void OnLocalizationPathSelected(const FString& Path);


	UFUNCTION(BlueprintImplementableEvent)
	void FillLocalizationName(const FString& LocName);

	UFUNCTION(BlueprintImplementableEvent)
	void FillAvailableLangs(const TArray<FString>& Langs);

	UFUNCTION(BlueprintImplementableEvent)
	void FillAvailableLangsInLocFile(const TArray<FString>& Langs);

	UFUNCTION(BlueprintImplementableEvent)
	void FillCSVPath(const FString& CSVPath);


	UFUNCTION(BlueprintCallable)
	void SelectNewCSVPath();

	UFUNCTION(BlueprintCallable)
	void GenerateLocFiles();


	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationPreview(bool LocalizationPreview);

	UFUNCTION(BlueprintCallable)
	void OnLocalizationPreviewChanged(bool LocalizationPreview);


	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationPreviewLang(const FString& PreviewLang);

	UFUNCTION(BlueprintCallable)
	void OnLocalizationPreviewLangChanged(const FString& PreviewLang);


	UFUNCTION(BlueprintImplementableEvent)
	void OnSetReimportAtEditorStartupChanged(bool bReimportAtEditorStartup);

	UFUNCTION(BlueprintCallable)
	void SetReimportAtEditorStartup(bool bReimportAtEditorStartup);


	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationOnFirstRun(bool LocalizationOnFirstRun);

	UFUNCTION(BlueprintCallable)
	void OnLocalizationOnFirstRun(bool LocalizationOnFirstRun);


	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalizationOnFirstRunLang(const FString& OnFirstRunLang);
	
	UFUNCTION(BlueprintCallable)
	void OnLocalizationOnFirstRunLangChanged(const FString& OnFirstRunLang);


	UFUNCTION(BlueprintImplementableEvent)
	void SetGlobalNamespace(const FString& GlobalNamespace);

	UFUNCTION(BlueprintCallable)
	void OnGlobalNamespaceChanged(const FString& NewGlobalNamespace);


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