// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LocText.h"
#include "ELTBlueprintLibrary.generated.h"

/**
 * Blueprint Function Library which exposes core Easy Localization Tool functionalities.
 */

DECLARE_DYNAMIC_DELEGATE(FELTTestDelegate);

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

	/**
	 * Casts FLocText to FText.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToText (LocText)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|Text")
	static FText Conv_LocTextToText(FLocText InLocText);

	/**
	 * Casts FLocText to FString.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (LocText)", CompactNodeTitle = "->", BlueprintAutocast), Category = "Utilities|Text")
	static FString Conv_LocTextToString(FLocText InLocText);

	/**
	 * Gets Package, Namespace and Key from a FText.
	 */
	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	static void GetTextData(FText InText, FString& OutPackage, FString& OutNamespace, FString& OutKey);
};
