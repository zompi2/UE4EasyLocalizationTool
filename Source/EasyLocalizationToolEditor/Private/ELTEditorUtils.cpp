// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#include "ELTEditorUtils.h"
#include "ELTBlueprintLibrary.h"
#include "UObject/Package.h"
#include "Internationalization/TextPackageNamespaceUtil.h"

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

void UELTEditorUtils::ReplaceText(UObject* OriginTextOwner, FText& OriginText, const FText& ReplaceWithText)
{
	if (ReplaceWithText.IsEmpty())
	{
		OriginText = FText::GetEmpty();
		return;
	}

	FString OriginPackage, OriginNamespace, OriginKey, OriginSource;
	UELTBlueprintLibrary::GetTextData(OriginText, OriginPackage, OriginNamespace, OriginKey, OriginSource);

	if (OriginPackage.IsEmpty())
	{
		if (UPackage* Package = OriginTextOwner->GetOutermost())
		{
			OriginPackage = TEXT("[") + TextNamespaceUtil::GetPackageNamespace(Package) + TEXT("]");
		}
	}

	FString ReplaceWithPackage, ReplaceWithNamespace, ReplaceWithKey, ReplaceWithSource;
	UELTBlueprintLibrary::GetTextData(ReplaceWithText, ReplaceWithPackage, ReplaceWithNamespace, ReplaceWithKey, ReplaceWithSource);

	FString NewNamespace = ReplaceWithNamespace + TEXT(" ") + OriginPackage;
	OriginText = FText::ChangeKey(NewNamespace, ReplaceWithKey, ReplaceWithText);
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION
