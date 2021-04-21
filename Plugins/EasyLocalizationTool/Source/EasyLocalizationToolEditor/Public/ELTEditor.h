// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTEditor.generated.h"

/**
 * 
 */

UCLASS()
class EASYLOCALIZATIONTOOLEDITOR_API UELTEditor : public UObject
{

	GENERATED_BODY()

public:

	void Init();
	bool CanCreateEditorUI();
	TSharedRef<class SWidget> CreateEditorUI();
	TWeakPtr<class SDockTab> EditorTab;
	static bool GenerateLocFilesImpl(const FString& CSVPath, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, FString& OutMessage);

private:

	TSharedRef<class SWidget> CreateEditorWidget();
	void ChangeTabWorld(UWorld* World, EMapChangeType MapChangeType);
	void InitializeTheWidget();

	void OnLocalizationPathChanged(const FString& NewPath);
	void OnCSVPathChanged(const FString& NewPath);
	void OnGenerateLocFiles();
	void OnReimportAtEditorStartupChanged(bool bNewReimportAtEditorStartup);
	void OnLocalizationPreviewChanged(bool bNewLocalizationPreview);
	void OnLocalizationPreviewLangChanged(const FString& LangPreview);
	void OnLocalizationFirstRunChanged(bool bNewLocalizationPreview);
	void OnLocalizationFirstRunLangChanged(const FString& LangPreview);
	void OnGlobalNamespaceChanged(const FString& NewGlobalNamespace);

	void SetLanguagePreview();
	void RefreshAvailableLangs(bool bRefreshUI);
	bool GenerateLocFiles(FString& OutMessage);

	const FString& GetCurrentCSVPath();
	const FString GetCurrentLocName();
	const FString GetCurrentGlobalNamespace();

	FString CurrentLocPath;
	TArray<FString> CurrentAvailableLangs;
	TArray<FString> CurrentAvailableLangsForLocFile;
	TMap<FString, FString> CSVPaths;

	UPROPERTY()
	class UELTEditorWidget* EditorWidget;


	class UEditorUtilityWidgetBlueprint* GetUtilityWidgetBlueprint();
};
