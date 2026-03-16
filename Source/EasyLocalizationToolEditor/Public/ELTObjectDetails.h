// Copyright (c) 2026 Damian Nowakowski. All rights reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

/**
 * Because the FObjectDetails is private, we have no other option but
 * copy this class and use it as a base for our custom details panel for UObject classes.
 * If at any point FObjectDetails become public, FTextLocPreview should inherit from it 
 * instead of this class.
 */

#pragma once

#if ELTEDITOR_WITH_PREVIEW_IN_UI

#include "Containers/Array.h"
#include "IDetailCustomization.h"
#include "Input/Reply.h"
#include "Templates/SharedPointer.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

class IDetailLayoutBuilder;
class UFunction;
class UObject;

class EASYLOCALIZATIONTOOLEDITOR_API FELTObjectDetails : public IDetailCustomization
{
public:
	// Creates an instance of FObjectDetails
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
	// End of IDetailCustomization interface

private:
	// Adds a warning banner if the class is marked as Experimental or EarlyAccessPreview
	void AddExperimentalWarningCategory(IDetailLayoutBuilder& DetailBuilder);

	// Creates a button strip in each category that contains reflected functions marked as CallInEditor
	void AddCallInEditorMethods(IDetailLayoutBuilder& DetailBuilder);

	TArray<TWeakObjectPtr<UObject>> GetFunctionCallExecutionContext(TWeakObjectPtr<UFunction> InWeakFunction) const;

private:
	// The list of selected objects, used when invoking a CallInEditor method
	TArray<TWeakObjectPtr<UObject>> SelectedObjectsList;
};


#endif // ELTEDITOR_WITH_PREVIEW_IN_UI
