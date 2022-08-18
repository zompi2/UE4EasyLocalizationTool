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
	 * are the same. It will return false if given FText is empty or if it is Culture Invariant.
	 */
	UFUNCTION(BlueprintPure, Category = "Editor Scripting | Easy Localization Tool")
	static bool ValidateText(FText InText);
};