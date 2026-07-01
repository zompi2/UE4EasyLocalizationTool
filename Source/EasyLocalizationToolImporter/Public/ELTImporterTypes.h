// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTImporterTypes.generated.h"

// Define the type of behavior when the localized string in CSV is empty and the fallback value should be used. 
UENUM(BlueprintType)
enum class EFallbackWhenEmptyType : uint8
{
	NONE,
	FIRST_LANG,
	KEY
};
