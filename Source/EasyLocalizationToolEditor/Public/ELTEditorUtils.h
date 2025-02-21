// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

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
	UFUNCTION(BlueprintPure, meta = (DisplayName = "ELT - Validate Text"), Category = "Editor Scripting | Easy Localization Tool")
	static bool ValidateText(FText InText);

	/**
	 * Replaces the OriginText with ReplaceWithText while keeping the OriginText's package id.
	 * Use this instead of normal FText copy operator when using editor scripts, because it
	 * copies the package id which leads to broken FText localizations.
	 * OriginTextOwner might be required when the OriginText is empty and has not package id assigned yet.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ELT - Replace Text"), Category = "Editor Scripting | Easy Localization Tool")
	static void ReplaceText(UObject* OriginTextOwner, UPARAM(ref) FText& OriginText, const FText& ReplaceWithText);
};