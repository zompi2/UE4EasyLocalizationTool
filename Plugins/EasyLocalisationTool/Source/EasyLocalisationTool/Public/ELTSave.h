// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "GameFramework/SaveGame.h"
#include "ELTSave.generated.h"


UCLASS()
class EASYLOCALISATIONTOOL_API UELTSave : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString CurrentLanguage;
};