// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#if ELTEDITOR_WITH_PREVIEW_IN_UI

#include "ELTObjectDetails.h"
#include "IDetailCustomization.h"
#include "UObject/UnrealType.h"
#include "UObject/WeakFieldPtr.h"
#include "EdGraphUtilities.h"
#include "DetailCategoryBuilder.h"

class EASYLOCALIZATIONTOOLEDITOR_API FTextLocPreview : public FELTObjectDetails
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	/** The blueprint we are editing */
	TWeakObjectPtr<UBlueprint> BlueprintPtr;
};

struct EASYLOCALIZATIONTOOLEDITOR_API FTextPreviewGraphPanelPinFactory : public FGraphPanelPinFactory
{
public:

	/** Create a custom pin for the FText preview in BP nodes. */
	TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
};

#endif // ELTEDITOR_WITH_PREVIEW_IN_UI
