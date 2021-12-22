// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

/**
 * Structure for storing Namespace and Key which will be used
 * to get localized FText. 
 * We need this, because FText doesn't work properly in inherited blueprints.
 */

#include "CoreMinimal.h"
#include "Internationalization/TextKey.h"
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

	UPROPERTY(EditAnywhere)
	FString Namespace;

	UPROPERTY(EditAnywhere)
	FString Key;

	/**
	 * Get localized FText based on Namespace and Key.
	 */
	FText GetText()
	{
		const FTextId TextId(*Namespace, *Key);
		FText OutText;
		FText::FindText(TextId.GetNamespace(), TextId.GetKey(), OutText, &Key);
		return OutText;
	}

	/**
	 * Get localized FString based on Namespace and Key.
	 */
	FString ToString()
	{
		return GetText().ToString();
	}
};