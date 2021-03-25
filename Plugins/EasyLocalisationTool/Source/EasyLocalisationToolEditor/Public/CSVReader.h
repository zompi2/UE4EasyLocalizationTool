// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/FileHelper.h"

struct FCSVColumn
{
	TArray<FString> Values;
	FCSVColumn(const FString& _FirstVal)
	{
		Values.Add(_FirstVal);
	}
};


class FCSVReader 
{
public:
	TArray<FCSVColumn> Columns;
	bool LoadFromFile(const FString& FilePath, FString& OutMessage)
	{
		FString FileContent;
		if (FFileHelper::LoadFileToString(FileContent, *FilePath))
		{
			Columns.Empty();
			TArray<TCHAR> Chars = FileContent.GetCharArray();

			bool bFirstLine = true;
			bool bInQuote = false;
			FString CurrentWord = TEXT("");
			int32 LineIndex = 0;

			auto AddWord = [this, &LineIndex , &bFirstLine, &CurrentWord]() -> bool
			{
				if (bFirstLine)
				{
					Columns.Add(FCSVColumn(CurrentWord));
				}
				else
				{
					if (Columns.IsValidIndex(LineIndex))
					{
						Columns[LineIndex].Values.Add(CurrentWord);
					}
					else
					{
						return false;
					}
				}
				return true;
			};

			for (int i = 0; i < Chars.Num(); i++)
			{
				const TCHAR& Ch = Chars[i];

				if (Ch == '\0' || i == Chars.Num() - 1)
				{
					if (AddWord() == false)
					{
						OutMessage = TEXT("ERROR: Invalid CSV!");
						return false;
					}
				}
				else if (Ch == '"')
				{	
					const TCHAR& Ch2 = Chars.IsValidIndex(i + 1) ? Chars[i + 1] : TCHAR(' ');
					if (Ch2 == '"')
					{
						CurrentWord.AppendChar('"');
						i++;
					}
					else
					{
						bInQuote = !bInQuote;
					}
				}
				else if (bInQuote == false)
				{
					if (Ch != '\r')
					{
						if (Ch == ',')
						{
							if (AddWord() == false)
							{
								OutMessage = TEXT("ERROR: Invalid CSV!");
								return false;
							}
							LineIndex++;
							CurrentWord.Empty();
						}
						else if (Ch == '\n')
						{
							if (AddWord() == false)
							{
								OutMessage = TEXT("ERROR: Invalid CSV!");
								return false;
							}
							if (bFirstLine)
							{
								bFirstLine = false;
							}
							LineIndex = 0;
							CurrentWord.Empty();
						}
						else
						{
							CurrentWord.AppendChar(Ch);
						}
					}
				}
				else
				{
					CurrentWord.AppendChar(Ch);
				}
			}
			return true;
		}
		else
		{
			OutMessage = TEXT("ERROR: CSV file not found!");
			return false;
		}
	}
};
