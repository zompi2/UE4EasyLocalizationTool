// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#if ELTEDITOR_WITH_PREVIEW_IN_UI

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "UObject/UnrealType.h"
#include "UObject/WeakFieldPtr.h"
#include "EdGraphUtilities.h"

class EASYLOCALIZATIONTOOLEDITOR_API FTextLocPreview : public IDetailCustomization
{
public:

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	/** The blueprint we are editing */
	TWeakObjectPtr<UBlueprint> BlueprintPtr;

	// Creates a button strip in each category that contains reflected functions marked as CallInEditor
	void AddCallInEditorMethods(IDetailLayoutBuilder& DetailBuilder);

	TArray<TWeakObjectPtr<UObject>> GetFunctionCallExecutionContext(TWeakObjectPtr<UFunction> InWeakFunction) const;

	// The list of selected objects, used when invoking a CallInEditor method
	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList;

private:

	FText EditedText;
	TSharedPtr<class IPropertyHandle> TextPropHandle;
};

struct EASYLOCALIZATIONTOOLEDITOR_API FTextPreviewGraphPanelPinFactory : public FGraphPanelPinFactory
{
public:

	TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
};

#endif // ELTEDITOR_WITH_PREVIEW_IN_UI
