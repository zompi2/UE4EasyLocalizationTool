// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTEditor.generated.h"

/**
 * Editor object handles all of the logic of the Easy Localization Tool.
 * It's tasks are to create a widget which is put into the dock, it also 
 * reacts to the user's interaction with created widget.
 * It sets up values like CSV path and global namespace and it actually
 * parses given CSV into the Unreal localization data.
 */

EASYLOCALIZATIONTOOLEDITOR_API DECLARE_LOG_CATEGORY_EXTERN(ELTEditorLog, Log, All);

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
	 * Sets up the EditorTab. Used by Editor Module right after a Tab is created.
	 */
	void SetEditorTab(const TSharedRef<class SDockTab>& NewEditorTab);

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
	static bool GenerateLocFilesImpl(const FString& CSVPaths, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, const TCHAR& Delimeter, FString& OutMessage);
	static bool GenerateLocFilesImpl(const TArray<FString>& CSVPaths, const FString& LocPath, const FString& LocName, const FString& GlobalNamespace, const TCHAR& Delimeter, FString& OutMessage);

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


	// ~~~~~~~~~ Events received from the Widget

	/**
	 * Called when Localization path has changed in the Widget.
	 */
	void OnLocalizationPathChanged(const FString& NewPath);

	/**
	 * Called when CSV path has changed in the Widget.
	 */
	void OnCSVPathChanged(const TArray<FString>& NewPaths);

	/**
	 * Called when the generate Loc Files request has been received from the Widget.
	 */
	void OnGenerateLocFiles();

	/**
	 * Called when "ReimportAtEditorStartup" option has been changed in the widget.
	 */
	void OnReimportAtEditorStartupChanged(bool bNewReimportAtEditorStartup);

	/**
	 * Called when "PreviewLocalization" option has been changed in the Widget.
	 */
	void OnLocalizationPreviewChanged(bool bNewLocalizationPreview);

	/**
	 * Called when "PreviewLanguage" option has been changed in the Widget.
	 */
	void OnLocalizationPreviewLangChanged(const FString& LangPreview);

	/**
	 * Called when "ManualLastLanguageLoad" option has been changed in the widget.
	 */
	void OnManuallySetLastLanguageChanged(bool bNewManuallySetLastLanguageLoad);

	/**
	 * Called when "OverrideLanguageOnFirstRun" option has been changed in the Widget.
	 */
	void OnLocalizationFirstRunChanged(bool bOnFirstRun);

	/**
	 * Called when "LanguageToOverrideAtFirstLaunch" option has been changed in the Widget.
	 */
	void OnLocalizationFirstRunLangChanged(const FString& LangOnFirstRun);

	/**
	 * Called when "GlobalNamespace" option has been changed in the Widget.
	 */
	void OnGlobalNamespaceChanged(const FString& NewGlobalNamespace);

	/**
	 * Called when "LogDebug" option has been changed in the Widget.
	 */
	void OnLogDebugChanged(bool bNewLogDebug);

	// ~~~~~~~~~ End of events received from the Widget


	/**
	 * Enables or disabled language preview in the editor based on the current settings.
	 */
	void SetLanguagePreview();

	/**
	 * Refresh the list of available languages based on the files that exists in Localization directory.
	 * It can optionally RefreshUI.
	 */
	void RefreshAvailableLangs(bool bRefreshUI);

	/**
	 * Generates Localization Files based on the given CSV path and Global Namespace.
	 * Returns false if the generation fails. OutMessage will contain a descriptive failure reason.
	 */
	bool GenerateLocFiles(FString& OutMessage);


	/**
	 * Returns the path to the CSV file for this Localization directory.
	 */
	const FString& GetCurrentCSVPath();

	/**
	 * Returns the currently selected Localization name (the Localization directory).
	 */
	FString GetCurrentLocName();

	/**
	 * Returns the Global Namespace set for this Localization directory.
	 */
	FString GetCurrentGlobalNamespace();

	/**
	 * Converts a list of relative paths to a list of absolute paths.
	 */
	static TArray<FString> RelativeToAbsolutePaths(const TArray<FString>& RelativePaths);

	/**
	 * Converst a list of paths into one string with ; separator.
	 */
	static FString PathsListToString(const TArray<FString>& Paths);

	/**
	 * Converts a string of paths with ; separator into a list of paths.
	 */
	static TArray<FString> PathsStringToList(const FString& Paths);

	// Handler of the created Editor Utility Widget. 
	// Is created in CreateEditorWidget().
	UPROPERTY()
	class UELTEditorWidget* EditorWidget;

	// A pointer to the EditorTab in which the editor widget should be docked. 
	// It is set by an Editor Module by SetEditorTab() right after the dock is created.
	TWeakPtr<class SDockTab> EditorTab;

	// Currently selected Localization directory path.
	FString CurrentLocPath;

	// List of overall available languages.
	TArray<FString> CurrentAvailableLangs;

	// List of languages available for currently selected Localization directory.
	TArray<FString> CurrentAvailableLangsForLocFile;

	// Cache of the CSV Paths defined for each Localization directory.
	TMap<FString, FString> CSVPaths;
};
