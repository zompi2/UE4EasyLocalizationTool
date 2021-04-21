// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTSettings.generated.h"

/**
 * Localization settings which are available for a game. 
 * Stored in DefaultConfig.ini file.
 */

UCLASS(config = Game, defaultconfig)
class EASYLOCALIZATIONTOOL_API UELTSettings : public UObject
{
    GENERATED_BODY()

public:

	/**
	 * Get/Set an option, which indicates if the language should be overridden 
	 * on a first game launch. Otherwise the local machine language will be used.
	 */
    static bool GetOverrideLanguageAtFirstLaunch();
    static void SetOverrideLanguageAtFirstLaunch(bool bNewOverridLanguageAtFirstLaunch);

	/**
	 * Get/Set a language, which will be used on a first game launch. Works only when
	 * OverrideLanguageAtFirstLaunch option is enabled.
	 */
	static FString GetLanguageToOverrideAtFirstLaunch();
	static void SetLanguageToOverrideAtFirstLaunch(FString NewLanguageToOverrideArFirstLaunch);

	/**
	 * Get/Set a list of available languages.
	 */
	static TArray<FString> GetAvailableLanguages();
	static void SetAvailableLanguages(const TArray<FString>& NewAvailableLanguages);

private:

	UPROPERTY(config)
	bool bOverrideLanguageAtFirstLaunch = false;

	UPROPERTY(config)
	FString LanguageToOverrideAtFirstLaunch = TEXT("");

	UPROPERTY(config)
	TArray<FString> AvailableLangs;
};