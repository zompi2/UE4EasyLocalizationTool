// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ELTBlueprintLibrary.generated.h"

/**
 * Blueprint Function Library which exposes core Easy Localization Tool functionalities.
 */

UCLASS()
class EASYLOCALIZATIONTOOL_API UELTBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Returns code for currently used language.
	 */
	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	static FString GetCurrentLanguage();

	/**
	 * Returns a list of codes of all available languages.
	 */
	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	static TArray<FString> GetAvailableLanguages();

	/**
	 * Checks if a language of given code can be set.
	 */
	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	static bool CanSetLanguage(const FString& Language);

	/**
	 * Sets a language represented by a given code.
	 * Returns true if language was set with success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool")
	static bool SetLanguage(const FString& Language);
};
