// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "GameFramework/SaveGame.h"
#include "ELTSave.generated.h"

/**
 * Simple save file to store a current language value.
 */

UCLASS()
class EASYLOCALIZATIONTOOL_API UELTSave : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString SavedCurrentLanguage;
};