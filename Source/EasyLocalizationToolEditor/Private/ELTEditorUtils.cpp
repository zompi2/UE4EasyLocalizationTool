// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

#include "ELTEditorUtils.h"
#include "ELTBlueprintLibrary.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

bool UELTEditorUtils::ValidateText(FText InText)
{
	if (InText.IsEmpty())
	{
		return false;
	}

	if (InText.IsCultureInvariant())
	{
		return false;
	}

	FString Package, Namespace, Key, Source;
	UELTBlueprintLibrary::GetTextData(InText, Package, Namespace, Key, Source);
	return Key.Equals(Source, ESearchCase::CaseSensitive);
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION