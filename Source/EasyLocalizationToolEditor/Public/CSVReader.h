// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * A class for parsing CSV files. Because it is easier for us to handle localizations
 * with data in columns it will store localization data in a column fashion struct.
 */

 /**
  * A struct which keeps CSV data in columns.
  */
struct FCSVColumn
{
	TArray<FString> Values;
	FCSVColumn(const FString& FirstValue)
	{
		Values.Add(FirstValue);
	}
};

/**
 * A version of CSV reader which doesn't use any 3rd party parsers.
 * For now it seems to be more suitable solution, as the ue4 parser doesn't
 * handle all possible exceptions.
 */
class FCSVReader
{
public:
	TArray<FCSVColumn> Columns;
	bool LoadFromFile(const FString& FilePath, const TCHAR& Separator, FString& OutMessage);
};
