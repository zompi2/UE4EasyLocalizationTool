// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#pragma once

#include "CSVReader.h"
#include "Misc/FileHelper.h"

#if ELT_USE_UNREAL_CSV_PARSER
#include "Serialization/Csv/CsvParser.h"
#endif

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

#if ELT_USE_UNREAL_CSV_PARSER

bool FCSVReader::LoadFromFile(const FString& FilePath, FString& OutMessage)
{
	FString FileContent;
	if (FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		const FCsvParser Parser(FileContent);
		const auto& Rows = Parser.GetRows();
		if (Rows.Num() > 0)
		{
			bool bFirstLine = true;
			Columns.Empty();
			int32 LineIndex = 0;
			for (const auto& Row : Rows)
			{
				for (const auto& Entry : Row)
				{
					if (bFirstLine)
					{
						Columns.Add(FCSVColumn(Entry));
					}
					else
					{
						if (Columns.IsValidIndex(LineIndex))
						{
							Columns[LineIndex].Values.Add(Entry);
						}
						else
						{
							OutMessage = TEXT("ERROR: Invalid CSV!");
							return false;
						}
						LineIndex++;
					}
				}
				bFirstLine = false;
				LineIndex = 0;
			}
			return true;
		}
		else
		{
			OutMessage = TEXT("ERROR: Invalid CSV!");
			return false;
		}
	}
	OutMessage = TEXT("ERROR: CSV file not found!");
	return false;
}

#else // ELT_USE_UNREAL_CSV_PARSER

bool FCSVReader::LoadFromFile(const FString& FilePath, FString& OutMessage)
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

		auto AddWord = [this, &LineIndex, &bFirstLine, &CurrentWord]() -> bool
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
				if (CurrentWord.IsEmpty())
				{
					return true;
				}
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
						if (CurrentWord.IsEmpty() && LineIndex == 0)
						{
							return true;
						}
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

#endif // ELT_USE_UNREAL_CSV_PARSER

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION