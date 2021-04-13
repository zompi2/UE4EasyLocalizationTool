// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ELTBlueprintLibrary.generated.h"

UCLASS()
class EASYLOCALIZATIONTOOL_API UELTBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	static FString GetCurrentLanguage();

	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	static TArray<FString> GetAvailableLanguages();

	UFUNCTION(BlueprintPure, Category = "Easy Localization Tool")
	static bool CanSetLanguage(const FString& Language);

	UFUNCTION(BlueprintCallable, Category = "Easy Localization Tool")
	static bool SetLanguage(const FString& Language);
};
