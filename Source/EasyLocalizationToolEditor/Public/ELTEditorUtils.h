// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#pragma once
 
#include "CoreMinimal.h"
#include "ELTEditorUtils.generated.h"

/**
 * Blueprint Function Library for Editor Scripting.
 */

UCLASS(Transient)
class EASYLOCALIZATIONTOOLEDITOR_API UELTEditorUtils : public UObject
{
	GENERATED_BODY()

public:
	
	/**
	 * Checks if the given FText is properly localized - it means it checks if Source and Key
	 * are the same. It will return false if given FText is empty.
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Scripting | Easy Localization Tool")
	static bool ValidateText(FText InText);

	/**
	 * Fixes the given FText if it is not properly localized. It will return the fixed FText via
	 * OutText parameter. It will return false if the InText is empty or it doesn't have to be fixed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Editor Scripting | Easy Localization Tool")
	static bool FixText(FText InText, FText& OutText);
};