// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#include "LocText.h"
#include "ELT.h"

FText FLocText::GetText()
{
	if (ValueCache.IsEmpty())
	{
		ValueCache = UELT::GetLocalizedText(Namespace, Key);
	}
	return ValueCache;
}
