// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blutility/Classes/EditorUtilityWidget.h"
#include "ELTEditorWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnLocalisationPathSelected, const FString&);
DECLARE_DELEGATE_OneParam(FOnCSVPathChanged, const FString&);
DECLARE_DELEGATE(FOnGenerateLocFiles);
DECLARE_DELEGATE_OneParam(FOnReimportAtEditorStartupChanged, bool);
DECLARE_DELEGATE_OneParam(FOnLocalisationPreviewChanged, bool);
DECLARE_DELEGATE_OneParam(FOnLocalisationPreviewLangChanged, const FString&);
DECLARE_DELEGATE_OneParam(FOnLocalisationOnFirstRunChanged, bool);
DECLARE_DELEGATE_OneParam(FOnLocalisationOnFirstRunLangChanged, const FString&);
DECLARE_DELEGATE_OneParam(FOnGlobalNamespaceChanged, const FString&);

UCLASS()
class EASYLOCALISATIONTOOLEDITOR_API UELTEditorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void FillLocalisationPaths(const TArray<FString>& Paths);

	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalisationPath(const FString& Path);

	UFUNCTION(BlueprintCallable)
	void OnLocalisationPathSelected(const FString& Path);


	UFUNCTION(BlueprintImplementableEvent)
	void FillLocalisationName(const FString& LocName);

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
	void SetLocalisationPreview(bool LocalisationPreview);

	UFUNCTION(BlueprintCallable)
	void OnLocalisationPreviewChanged(bool LocalisationPreview);


	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalisationPreviewLang(const FString& PreviewLang);

	UFUNCTION(BlueprintCallable)
	void OnLocalisationPreviewLangChanged(const FString& PreviewLang);


	UFUNCTION(BlueprintImplementableEvent)
	void OnSetReimportAtEditorStartupChanged(bool bReimportAtEditorStartup);

	UFUNCTION(BlueprintCallable)
	void SetReimportAtEditorStartup(bool bReimportAtEditorStartup);


	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalisationOnFirstRun(bool LocalisationOnFirstRun);

	UFUNCTION(BlueprintCallable)
	void OnLocalisationOnFirstRun(bool LocalisationOnFirstRun);


	UFUNCTION(BlueprintImplementableEvent)
	void SetLocalisationOnFirstRunLang(const FString& OnFirstRunLang);
	
	UFUNCTION(BlueprintCallable)
	void OnLocalisationOnFirstRunLangChanged(const FString& OnFirstRunLang);


	UFUNCTION(BlueprintImplementableEvent)
	void SetGlobalNamespace(const FString& GlobalNamespace);

	UFUNCTION(BlueprintCallable)
	void OnGlobalNamespaceChanged(const FString& NewGlobalNamespace);


	FOnLocalisationPathSelected OnLocalisationPathSelectedDelegate;
	FOnCSVPathChanged OnCSVPathChangedDelegate;
	FOnGenerateLocFiles OnGenerateLocFilesDelegate;
	FOnReimportAtEditorStartupChanged OnReimportAtEditorStartupChangedDelegate;
	FOnLocalisationPreviewChanged OnLocalisationPreviewChangedDelegate;
	FOnLocalisationPreviewLangChanged OnLocalisationPreviewLangChangedDelegate;
	FOnLocalisationOnFirstRunChanged OnLocalisationOnFirstRunChangedDelegate;
	FOnLocalisationOnFirstRunLangChanged OnLocalisationOnFirstRunLangChangedDelegate;
	FOnGlobalNamespaceChanged OnGlobalNamespaceChangedDelegate;
};