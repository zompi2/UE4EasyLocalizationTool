// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

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
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", DisplayName = "ELT - Get Current Language"), Category = "Easy Localization Tool")
	static FString GetCurrentLanguage(const UObject* WorldContextObject);

	/**
	 * Returns a list of codes of all available languages.
	 */
	UFUNCTION(BlueprintPure, meta = (DeprecatedFunction, DeprecationMessage="This function is deprecated. Use UELT::GetAvailableLanguages() instead.", WorldContext = "WorldContextObject", DisplayName = "ELT - Get Available Languages"), Category = "Easy Localization Tool")
	static TArray<FString> GetAvailableLanguages(const UObject* WorldContextObject);

	/**
	 * Checks if a language of given code can be set.
	 */
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject", DisplayName = "ELT - Can Set Language"), Category = "Easy Localization Tool")
	static bool CanSetLanguage(const UObject* WorldContextObject, const FString& Language);

	/**
	 * Sets a language represented by a given code.
	 * Returns true if language was set with success.
	 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "ELT - Set Language"), Category = "Easy Localization Tool")
	static bool SetLanguage(const UObject* WorldContextObject, const FString& Language);

	/**
	 * Refreshes current language resources.
	 * Might be needed when some texts are not displayed properly.
	 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "ELT - Refresh Language Resources"), Category = "Easy Localization Tool")
	static void RefreshLanguageResources(const UObject* WorldContextObject);

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
	 * Gets Package, Namespace, Key ans Source from a FText.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ELT - Get Text Data"), Category = "Easy Localization Tool")
	static void GetTextData(FText InText, FString& OutPackage, FString& OutNamespace, FString& OutKey, FString& OutSource);

	/**
	 * Checks if two FTexts keys are the same. If at least one of them has invalid key it will return false.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ELT - Are Text Keys Equal"), Category = "Easy Localization Tool")
	static bool AreTextKeysEqual(const FText& A, const FText& B);

	/**
	 * Gets a buffer string from FText. It can be used to save FText data in a file or send it over the network. 
	 * The buffer contains Package, Namespace and Key. 
	 * bRequiresQuotes - True if the written text literal must be surrounded by quotes (eg, when saving as a delimited list)
	 * bStripPackage - Namespace True to strip the package namespace from the written NSLOCTEXT value (eg, when saving cooked data)
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ELT - Get Text As Buffer"), Category = "Easy Localization Tool")
	static FString GetTextAsBuffer(const FText& InText, const bool bRequiresQuotes = false, const bool bStripPackageNamespace = false);

	/**
	 * Creates FText from a buffer string created by GetTextAsBuffer. The buffer should contain Package, Namespace and Key.
	 * bRequiresQuotes - True if the read text literal must be surrounded by quotes (eg, when loading from a delimited list).
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ELT - Make Text From Buffer"), Category = "Easy Localization Tool")
	static FText MakeTextFromBuffer(const FString& InBuffer, const bool bRequiresQuotes = false);
};
