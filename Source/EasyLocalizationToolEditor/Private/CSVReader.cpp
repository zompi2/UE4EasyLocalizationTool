// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

#include "CSVReader.h"
#include "Misc/FileHelper.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

bool FCSVReader::LoadFromFile(const FString& FilePath, const TCHAR& Separator, FString& OutMessage)
{
	FString FileContent;
	if (FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		Columns.Empty();
		TArray<TCHAR> Chars = FileContent.GetCharArray();

		bool bInQuote = false;
		FString CurrentWord = TEXT("");
		int32 ColumnIndex = 0;
		int32 RowIndex = 0;

		auto AddWord = [this, &ColumnIndex, &RowIndex, &CurrentWord, &OutMessage]() -> bool
		{
			if (RowIndex == 0)
			{
				Columns.Add(FCSVColumn(CurrentWord));
			}
			else
			{
				if (Columns.IsValidIndex(ColumnIndex))
				{
					Columns[ColumnIndex].Values.Add(CurrentWord);
				}
				else
				{
					OutMessage = FString::Printf(TEXT("ERROR: Failed loading CSV! Trying to add a word: '%s' to a row %i, column %i (counting from 1) while there are %i columns."), *CurrentWord, RowIndex+1, ColumnIndex+1, Columns.Num());
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
					if (Ch == Separator)
					{
						if (AddWord() == false)
						{
							return false;
						}
						ColumnIndex++;
						CurrentWord.Empty();
					}
					else if (Ch == '\n')
					{
						if (CurrentWord.IsEmpty() && ColumnIndex == 0)
						{
							return true;
						}
						if (AddWord() == false)
						{
							return false;
						}
						ColumnIndex = 0;
						RowIndex++;
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

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION