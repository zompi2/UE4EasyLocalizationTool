// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Internationalization/TextLocalizationResource.h"

// Define the type of behavior when the localized string in CSV is empty and the fallback value should be used. 
UENUM(BlueprintType)
enum class EFallbackWhenEmptyType : uint8
{
	NONE,
	FIRST_LANG,
	KEY
};

class EASYLOCALIZATIONTOOLIMPORTER_API FELTImporter
{

public:

	/**
	 * Implementation of generating Unreal localization files. It is statically exposed,
	 * so other elements like Commandlet can run it.
	 */
	static bool GenerateLoc(const TArray<FString>& CSVPaths, 
							const FString& LocPath, 
							const FString& LocName, 
							const FString& GlobalNamespace, 
							const FString& Separator,
							EFallbackWhenEmptyType FallbackWhenEmpty,
							bool bGenerateStringTables, 
							bool bSaveToFiles,
							bool bLogDebug, 
							FString& OutMessage);

	/**
	 * Gets the name of the StringTable based on the LocName and Namespace.
	 */
	static FString GetStringTableName(const FString& LocName, const FString& Namespace);

	/**
	 * Converts Fallback type String to Enum.
	 */
	static EFallbackWhenEmptyType FallbackStringToEnum(const FString& String);

	/**
	 * Converts Fallback type Enum to String.
	 */
	static FString FallbackEnumToString(EFallbackWhenEmptyType Enum);

	static TMap<FString, FTextLocalizationResource> CachedResources;
};


