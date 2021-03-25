// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTEditor.generated.h"

class SWidget;

UCLASS()
class EASYLOCALISATIONTOOLEDITOR_API UELTEditor : public UObject
{
public:

	GENERATED_BODY()

	void Init();

	bool CanCreateEditorUI();
	TSharedRef<SWidget> CreateEditorUI();
	TSharedRef<SWidget> CreateEditorWidget();
	void ChangeTabWorld(UWorld* World, EMapChangeType MapChangeType);
	void InitializeTheWidget();

	void OnLocalisationPathChanged(const FString& NewPath);
	void OnCSVPathChanged(const FString& NewPath);
	void OnGenerateLocFiles();
	void OnReimportAtEditorStartupChanged(bool bNewReimportAtEditorStartup);
	void OnLocalisationPreviewChanged(bool bNewLocalisationPreview);
	void OnLocalisationPreviewLangChanged(const FString& LangPreview);
	void OnLocalisationFirstRunChanged(bool bNewLocalisationPreview);
	void OnLocalisationFirstRunLangChanged(const FString& LangPreview);
	void OnGlobalNamespaceChanged(const FString& NewGlobalNamespace);

	void SetLanguagePreview();
	void RefreshAvailableLangs(bool bRefreshUI);
	bool GenerateLocFiles(FString& OutMessage);
	static bool GenerateLocFilesImpl(const FString& CSVPath, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, FString& OutMessage);
	const FString& GetCurrentCSVPath();
	const FString GetCurrentLocName();
	const FString GetCurrentGlobalNamespace();

	FString CurrentLocPath;
	TArray<FString> CurrentAvailableLangs;
	TArray<FString> CurrentAvailableLangsForLocFile;
	TMap<FString, FString> CSVPaths;

	UPROPERTY()
	class UELTEditorWidget* EditorWidget;
	TWeakPtr<SDockTab> EditorTab;

	class UEditorUtilityWidgetBlueprint* GetUtilityWidgetBlueprint();
};
