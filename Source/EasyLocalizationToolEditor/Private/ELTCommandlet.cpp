// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#include "ELTCommandlet.h"
#include "ELTEditor.h"
#include "Misc/Parse.h"

DEFINE_LOG_CATEGORY_STATIC(ELTCommandletLog, Log, All);

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

// Usage:
// -run=ELTCommandlet -CSVPath="C:\MyProject\Lockit.csv" -LocPath="C:\MyProject\Content\Localization\Game" -Namespace="GAME"
int32 UELTCommandlet::Main(const FString& Params)
{
	UE_LOG(ELTCommandletLog, Log, TEXT("+++ Running Easy Localization Tool Commandlet!"));

	FString CSVPath;
	if (FParse::Value(*Params, TEXT("-CSVPath="), CSVPath) == false)
	{
		UE_LOG(ELTCommandletLog, Log, TEXT("+++ Failed to generate Localization. -CSVPath= not given!"));
		return 1;
	}

	FString LocPath;
	if (FParse::Value(*Params, TEXT("-LocPath="), LocPath) == false)
	{
		UE_LOG(ELTCommandletLog, Log, TEXT("+++ Failed to generate Localization. -LocPath= not given!"));
		return 1;
	}

	FString Namespace;
	FParse::Value(*Params, TEXT("-Namespace="), Namespace);

	FString Separator = TEXT(",");
	FParse::Value(*Params, TEXT("-Separator="), Separator);

	const FString LocName = FPaths::GetBaseFilename(LocPath);

	// Run generation of loc files implementation. Get the output message and display it the localization fails.
	FString OutMessage;
	if (UELTEditor::GenerateLocFilesImpl(CSVPath, LocPath, LocName, Namespace, Separator, OutMessage) == false)
	{
		UE_LOG(ELTCommandletLog, Log, TEXT("+++ Failed to generate Localization: %s"), *OutMessage);
		return 1;
	}

	return 0;
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION