// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "ELTCustomTextResource.h"
#include "ELT.h"
#include "ELTImporter.h"

ELT_PRAGMA_DISABLE_OPTIMIZATION

int32 FELTCustomTextResource::GetPriority() const
{
	return ELocalizedTextSourcePriority::High;
}

bool FELTCustomTextResource::GetNativeCultureName(const ELocalizedTextSourceCategory InCategory, FString& OutNativeCultureName)
{
	FString NativeCultureName = TextLocalizationResourceUtil::GetNativeCultureName(InCategory);
	if (!NativeCultureName.IsEmpty())
	{
		OutNativeCultureName = MoveTemp(NativeCultureName);
		return true;
	}
	return false;
}

void FELTCustomTextResource::GetLocalizedCultureNames(const ELocalizationLoadFlags InLoadFlags, TSet<FString>& OutLocalizedCultureNames)
{
	for (const FString& AvailableLang : UELT::GetAvailableLanguages())
	{
		OutLocalizedCultureNames.Add(AvailableLang);
	}
}

void FELTCustomTextResource::LoadLocalizedResources(const ELocalizationLoadFlags InLoadFlags, TArrayView<const FString> InPrioritizedCultures, FTextLocalizationResource& InOutNativeResource, FTextLocalizationResource& InOutLocalizedResource)
{
	if (ShouldLoadGame(InLoadFlags) == false)
	{
		return;
	}

	if (FELTImporter::CachedResources.IsEmpty())
	{
		return;
	}

	const FString NativeCultureName = TextLocalizationResourceUtil::GetNativeCultureName(ELocalizedTextSourceCategory::Game);
	if (NativeCultureName.IsEmpty() == false)
	{
		if (FTextLocalizationResource* FoundRes = FELTImporter::CachedResources.Find(NativeCultureName))
		{
			InOutNativeResource = *FoundRes;
		}
	}

	for (const FString& CultureName : InPrioritizedCultures)
	{
		if (FTextLocalizationResource* FoundRes = FELTImporter::CachedResources.Find(CultureName))
		{
			InOutLocalizedResource = *FoundRes;
			break;
		}
	}
}

ELT_PRAGMA_ENABLE_OPTIMIZATION