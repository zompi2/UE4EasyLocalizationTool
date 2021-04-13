// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTEditorSettings.generated.h"

UCLASS(config = Editor)
class EASYLOCALIZATIONTOOLEDITOR_API UELTEditorSettings : public UObject
{
    GENERATED_BODY()

public:

	static FString GetLocalizationPath();
	static void SetLocalizationPath(const FString& NewLocalizationPath);

	static TMap<FString, FString> GetCSVPaths();
	static void SetCSVPaths(const TMap<FString, FString>& NewCSVPaths);

	static bool GetLocalizationPreview();
	static void SetLocalizationPreview(bool bNewPreview);

	static FString GetLocalizationPreviewLang();
	static void SetLocalizationPreiewLang(const FString& NewLocalizationPreviewLang);

	static bool GetReimportAtEditorStartup();
	static void SetReimportAtEditorStartup(bool bNewReimportAtEditorStartup);

	static TMap<FString, FString> GetGlobalNamespaces();
	static void SetGlobalNamespace(const TMap<FString, FString>& NewGlobalNamespaces);

private:

	UPROPERTY(config)
	FString LocalizationPath = TEXT("");

	UPROPERTY(config)
	TMap<FString, FString> CSVPaths;

	UPROPERTY(config)
	bool bLocalizationPreview = false;

	UPROPERTY(config)
	FString LocalizationPreviewLang = TEXT("");

	UPROPERTY(config)
	bool bReimportAtEditorStartup = false;

	UPROPERTY(config)
	TMap<FString, FString> GlobalNamespaces;
};
