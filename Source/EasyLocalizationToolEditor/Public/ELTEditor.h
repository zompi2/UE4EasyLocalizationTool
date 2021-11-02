// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTEditor.generated.h"

/**
 * Editor object handles all of the logic of the Easy Localization Tool.
 * It's tasks are to create a widget which can be put into the dock, it also 
 * reacts to the user's interaction with created widget.
 * It sets up values like CSV path and global namespace and it actually
 * parses given CSV into the Unreal localization data.
 */

UCLASS()
class EASYLOCALIZATIONTOOLEDITOR_API UELTEditor : public UObject
{

	GENERATED_BODY()

public:

	/**
	 * Initializes the editor object. Runs right after
	 * it's creation.
	 */
	void Init();

	/**
	 * Returns true if the Editor UI widget can be created.
	 */
	bool CanCreateEditorUI();

	/**
	 * Creates Editor UI widget and returns a reference to it.
	 * It is called from the Editor Module in a moment of Tab Creation.
	 */
	TSharedRef<class SWidget> CreateEditorUI();
	
	/**
	 * Implementation of generating Unreal localization files. It is statically exposed, 
	 * so other elements like Commandlet can run it.
	 */
	static bool GenerateLocFilesImpl(const FString& CSVPath, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, FString& OutMessage);

	// A pointer to the EditorTab in which the editor widget should be docked. 
	// It is passed by an Editor Module right after the dock is created.
	TWeakPtr<class SDockTab> EditorTab;

private:

	/**
	 * Returns an Utility Widget Blueprint from Content directory which will
	 * be used to create an Editor options window. The widget must be located
	 * in the given in this function location.
	 */
	class UEditorUtilityWidgetBlueprint* GetUtilityWidgetBlueprint();

	/**
	 * Creates the Editor Utility Widget from the Utility Widget Blueprint.
	 * It can be called from CreateDitorUI() when it is opened by user,
	 * or it can be called from ChangeTabWorld() in a situation when a map is loaded 
	 * or created and an Editor Tab is valid (moving widget between maps).
	 */
	TSharedRef<class SWidget> CreateEditorWidget();

	/**
	 * Called when OnMapChanged event occurs. The EditorTab must be properly
	 * handled when the world is torn down or when a map is created or loaded.
	 */
	void ChangeTabWorld(UWorld* World, EMapChangeType MapChangeType);

	/**
	 * Initialize EditorWidget after it's creation. 
	 * Binds all required delegates and sets up default values to the Widget.
	 */
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

	// Handler of the created Editor Utility Widget. 
	// Is created in CreateEditorWidget().
	UPROPERTY()
	class UELTEditorWidget* EditorWidget;

	// 
	FString CurrentLocPath;
	TArray<FString> CurrentAvailableLangs;
	TArray<FString> CurrentAvailableLangsForLocFile;
	TMap<FString, FString> CSVPaths;


};
