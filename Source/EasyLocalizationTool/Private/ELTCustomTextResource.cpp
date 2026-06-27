
#include "ELTCustomTextResource.h"
#include "ELT.h"
#include "ELTImporter.h"

UE_DISABLE_OPTIMIZATION

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

	for (const FString& CultureName : InPrioritizedCultures)
	{
		if (FTextLocalizationResource* FoundRes = FELTImporter::CachedResources.Find(CultureName))
		{
			InOutNativeResource = *FoundRes;
			InOutLocalizedResource = *FoundRes;
			break;
		}
	}
}

UE_ENABLE_OPTIMIZATION