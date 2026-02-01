// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "UObject/UnrealType.h"
#include "UObject/WeakFieldPtr.h"

class EASYLOCALIZATIONTOOLEDITOR_API FTextLocPreview : public IDetailCustomization
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedPtr<IDetailCustomization> MakeInstanceBP(TSharedPtr<IBlueprintEditor> InBlueprintEditor);
	static TSharedRef<IDetailCustomization> MakeInstance();

	FTextLocPreview(UBlueprint* InBlueprintPtr);
	FTextLocPreview();

	// IDetailCustomization interface
	void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	/** The blueprint we are editing */
	TWeakObjectPtr<UBlueprint> BlueprintPtr;

private:

	FText EditedText;
};
