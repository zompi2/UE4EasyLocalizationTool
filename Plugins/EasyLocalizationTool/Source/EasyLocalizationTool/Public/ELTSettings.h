// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class EASYLOCALIZATIONTOOL_API UELTSettings : public UObject
{

    GENERATED_BODY()

public:

    static bool GetOverrideLanguageAtFirstLaunch();
    static void SetOverrideLanguageAtFirstLaunch(bool bNewOverridLanguageAtFirstLaunch);

	static FString GetLanguageToOverrideAtFirstLaunch();
	static void SetLanguageToOverrideAtFirstLaunch(FString NewLanguageToOverrideArFirstLaunch);

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