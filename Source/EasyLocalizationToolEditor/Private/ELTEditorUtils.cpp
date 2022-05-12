// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#include "ELTEditorUtils.h"
#include "ELTBlueprintLibrary.h"

bool UELTEditorUtils::ValidateText(FText InText)
{
	if (InText.IsEmpty())
	{
		return false;
	}

	FString Package, Namespace, Key, Source;
	UELTBlueprintLibrary::GetTextData(InText, Package, Namespace, Key, Source);
	return Key.Equals(Source, ESearchCase::CaseSensitive);
}

bool UELTEditorUtils::FixText(FText InText, FText& OutText)
{
	if (InText.IsEmpty() == false)
	{
		FString Package, Namespace, Key, Source;
		UELTBlueprintLibrary::GetTextData(InText, Package, Namespace, Key, Source);
		if (Key.Equals(Source, ESearchCase::CaseSensitive))
		{
			// The text is ok. Nothing to fix.
			OutText = InText;
			return false;
		}
		else
		{
			// Text is not ok, fix it.
			OutText = FText::ChangeKey(FTextKey(Namespace), FTextKey(Key), FText::FromString(Key));
			return true;
		}
	}
	return false;
}