// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "ELTCommandlet.generated.h"

/**
 * Commandlet which allows to run localization tool via command line without running an engine.
 */

UCLASS()
class EASYLOCALIZATIONTOOLEDITOR_API UELTCommandlet : public UCommandlet
{
public:

	GENERATED_BODY()

	/**
	 * Main function of commandlet launched when running command from command line.
	 */
	int32 Main(const FString& Params) override;
};
