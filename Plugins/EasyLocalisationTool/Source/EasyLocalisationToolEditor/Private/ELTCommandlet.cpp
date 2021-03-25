// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#include "ELTCommandlet.h"
#include "ELTEditor.h"
#include "Misc/Parse.h"

DEFINE_LOG_CATEGORY_STATIC(ELTCommandletLog, Log, All);

// Usage:
// -run=ELTCommandlet -CSVPath="C:\MyProject\Lockit.csv" -LocPath="C:\MyProject\Content\Localization\Game" -Namespace="GAME"
int32 UELTCommandlet::Main(const FString& Params)
{
	UE_LOG(ELTCommandletLog, Log, TEXT("+++ Running Easy Localisation Tool Commandlet!"));

	FString CSVPath;
	if (FParse::Value(*Params, TEXT("-CSVPath="), CSVPath) == false)
	{
		UE_LOG(ELTCommandletLog, Log, TEXT("+++ Failed to generate localisation. -CSVPath= not given!"));
		return 1;
	}

	FString LocPath;
	if (FParse::Value(*Params, TEXT("-LocPath="), LocPath) == false)
	{
		UE_LOG(ELTCommandletLog, Log, TEXT("+++ Failed to generate localisation. -LocPath= not given!"));
		return 1;
	}

	FString Namespace;
	FParse::Value(*Params, TEXT("-Namespace="), Namespace);

	const FString LocName = FPaths::GetBaseFilename(LocPath);

	FString OutMessage;
	if (UELTEditor::GenerateLocFilesImpl(CSVPath, LocPath, LocName, Namespace, OutMessage) == false)
	{
		UE_LOG(ELTCommandletLog, Log, TEXT("+++ Failed to generate localisation: %s"), *OutMessage);
		return 1;
	}

	return 0;
}