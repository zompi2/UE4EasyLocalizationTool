// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

/**
 * Structure for storing Namespace and Key which will be used
 * to get localized FText.
 * We need this, because FText doesn't work properly in inherited blueprints.
 */

#include "CoreMinimal.h"
#include "Internationalization/TextKey.h"
#include "Runtime/Launch/Resources/Version.h"
#include "LocText.generated.h"

USTRUCT(BlueprintType)
struct FLocText
{
	GENERATED_BODY()

	FLocText() = default;
	FLocText(const FString& InNamespace, const FString& InKey) :
		Namespace(InNamespace),
		Key(InKey)
	{}
	FLocText(FString&& InNamespace, FString&& InKey) :
		Namespace(MoveTemp(InNamespace)),
		Key(MoveTemp(InKey))
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Easy Localization Tool")
	FString Namespace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Easy Localization Tool")
	FString Key;

	/** A result text, cached for future uses */
	FText ValueCache;

	/**
	 * Get localized FText based on Namespace and Key.
	 */
	FText GetText()
	{
		if (ValueCache.IsEmpty())
		{
			const FTextId TextId(*Namespace, *Key);
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 5))
			FText::FindTextInLiveTable_Advanced(TextId.GetNamespace(), TextId.GetKey(), ValueCache, &Key);
#else
			FText::FindText(TextId.GetNamespace(), TextId.GetKey(), ValueCache, &Key);
#endif
			if (ValueCache.IsEmpty())
			{
				ValueCache = FText::FromString(Key);
			}
		}
		return ValueCache;
	}

	/**--
	 * Get localized FString based on Namespace and Key.
	 */
	FString ToString()
	{
		return GetText().ToString();
	}
};