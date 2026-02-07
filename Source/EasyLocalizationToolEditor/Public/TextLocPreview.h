// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 5)) 

#include "IDetailCustomization.h"
#include "UObject/UnrealType.h"
#include "UObject/WeakFieldPtr.h"

class EASYLOCALIZATIONTOOLEDITOR_API FTextLocPreview : public IDetailCustomization
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	/** The blueprint we are editing */
	TWeakObjectPtr<UBlueprint> BlueprintPtr;

private:

	FText EditedText;
	TSharedPtr<class IPropertyHandle> TextPropHandle;
};

#endif // #if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 5))
