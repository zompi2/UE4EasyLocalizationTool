// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTEditorSettings.generated.h"

/**
 * Localization settings which are available for an editor only.
 * Stored in DefaultEditor.ini file.
 */

UCLASS(config = Editor, defaultconfig)
class EASYLOCALIZATIONTOOLEDITOR_API UELTEditorSettings : public UObject
{
    GENERATED_BODY()

public:

	/**
	 * Get/Set a path where localization files are stored.
	 */
	static FString GetLocalizationPath();
	static void SetLocalizationPath(const FString& NewLocalizationPath);

	/**
	 * Get/Set paths where CSV files corresponded to a localization files paths.
	 */
	static TMap<FString, FString> GetCSVPaths();
	static void SetCSVPaths(const TMap<FString, FString>& NewCSVPaths);

	/**
	 * Get/Set global namespaces corresponded to a localization files path.
	 */
	static TMap<FString, FString> GetGlobalNamespaces();
	static void SetGlobalNamespace(const TMap<FString, FString>& NewGlobalNamespaces);

	/**
	 * Get/Set if the localization preview is on.
	 */
	static bool GetLocalizationPreview();
	static void SetLocalizationPreview(bool bNewPreview);

	/**
	 * Get/Set lang code to preview in editor.
	 */
	static FString GetLocalizationPreviewLang();
	static void SetLocalizationPreviewLang(const FString& NewLocalizationPreviewLang);

	/**
	 * Get/Set if the localizations should be reimported at the editor startup.
	 */
	static bool GetReimportAtEditorStartup();
	static void SetReimportAtEditorStartup(bool bNewReimportAtEditorStartup);

private:

	UPROPERTY(config)
	FString LocalizationPath = TEXT("");

	UPROPERTY(config)
	TMap<FString, FString> CSVPaths;

	UPROPERTY(config)
	TMap<FString, FString> GlobalNamespaces;

	UPROPERTY(config)
	bool bLocalizationPreview = false;

	UPROPERTY(config)
	FString LocalizationPreviewLang = TEXT("");

	UPROPERTY(config)
	bool bReimportAtEditorStartup = false;
};
