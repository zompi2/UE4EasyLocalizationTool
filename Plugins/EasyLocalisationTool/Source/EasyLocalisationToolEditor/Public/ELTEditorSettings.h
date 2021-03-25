// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTEditorSettings.generated.h"

UCLASS(config = Editor)
class EASYLOCALISATIONTOOLEDITOR_API UELTEditorSettings : public UObject
{
    GENERATED_BODY()

public:

	static FString GetLocalisationPath();
	static void SetLocalisationPath(const FString& NewLocalisationPath);

	static TMap<FString, FString> GetCSVPaths();
	static void SetCSVPaths(const TMap<FString, FString>& NewCSVPaths);

	static bool GetLocalisationPreview();
	static void SetLocalisationPreview(bool bNewPreview);

	static FString GetLocalisationPreviewLang();
	static void SetLocalisationPreiewLang(const FString& NewLocalisationPreviewLang);

	static bool GetReimportAtEditorStartup();
	static void SetReimportAtEditorStartup(bool bNewReimportAtEditorStartup);

	static TMap<FString, FString> GetGlobalNamespaces();
	static void SetGlobalNamespace(const TMap<FString, FString>& NewGlobalNamespaces);

private:

	UPROPERTY(config)
	FString LocalisationPath = TEXT("");

	UPROPERTY(config)
	TMap<FString, FString> CSVPaths;

	UPROPERTY(config)
	bool bLocalisationPreview = false;

	UPROPERTY(config)
	FString LocalisationPreviewLang = TEXT("");

	UPROPERTY(config)
	bool bReimportAtEditorStartup = false;

	UPROPERTY(config)
	TMap<FString, FString> GlobalNamespaces;
};
