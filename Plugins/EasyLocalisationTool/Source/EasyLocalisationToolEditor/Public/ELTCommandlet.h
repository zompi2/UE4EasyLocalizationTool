// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "ELTCommandlet.generated.h"

UCLASS()
class EASYLOCALISATIONTOOLEDITOR_API UELTCommandlet : public UCommandlet
{
public:

	GENERATED_BODY()

	int32 Main(const FString& Params) override;
};
