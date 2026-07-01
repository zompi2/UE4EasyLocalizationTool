// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ELTImporterTypes.h"
#include "Internationalization/TextLocalizationResource.h"

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
							bool bCache,
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

	// Cache of localization resources used for runtime reimport.
	static TMap<FString, FTextLocalizationResource> CachedResources;

	// Priority of the cached resources. Used to determine if the cache should be updated or not.
	static int32 CachedResourcesPriority;
};


