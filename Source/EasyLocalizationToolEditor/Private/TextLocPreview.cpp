// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#if ELTEDITOR_WITH_PREVIEW_IN_UI

#include "TextLocPreview.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ELTEditorSettings.h"
#include "SGraphPinTextPreview.h"
#include "PropertyCustomizationHelpers.h"
#include "ObjectEditorUtils.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

TSharedRef<IDetailCustomization> FTextLocPreview::MakeInstance()
{
	return MakeShareable(new FTextLocPreview);
}

void FTextLocPreview::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	// Call base class implementation to keep CallInEditor and Experimental category functionality
	FELTObjectDetails::CustomizeDetails(DetailLayout);

	// Do nothing if the preview in UI is disabled in settings
	if (UELTEditorSettings::GetPreviewInUIEnabled() == false)
	{
		return;
	}

	TArray<FName> CategoryNames;
	DetailLayout.GetCategoryNames(CategoryNames);
	for (const FName& CatName : CategoryNames)
	{
		// This will be called in the BP editor when editing the default value of the variable.
		if (CatName == "DefaultValueCategory")
		{
			// We can extend the visuals of it by using EditCategory.
			IDetailCategoryBuilder& Category = DetailLayout.EditCategory(CatName, NSLOCTEXT("BlueprintDetailsCustomization", "DefaultValueCategoryHeading", "Default Value"));
			TArray<TSharedRef<IPropertyHandle>> PropertyHandles;
			Category.GetDefaultProperties(PropertyHandles);
			for (TSharedRef<IPropertyHandle>& PropHandle : PropertyHandles)
			{
				// Ensure the handle is valid ...
				if (PropHandle->IsValidHandle() == false)
				{
					continue;
				}

				// ... and points to a FText property.
				if (FProperty* Prop = PropHandle->GetProperty())
				{
					if (Prop->IsA(FTextProperty::StaticClass()) == false)
					{
						continue;
					}
				}			

				// Add a custom row to the category with the preview widget.
				Category.AddCustomRow(INVTEXT("LocPreview"))
					.ValueContent()
					[
						SNew(SHorizontalBox)
							+SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(STextBlock).Text_Lambda([this, PropHandle]
								{
									FText TextValue;
									PropHandle->GetValue(TextValue);
									return TextValue;
								})
							]
					];
			}
		}
		// This will be called when editing the default value of a variable in the details panel of the UObject.
		// Can be in BP editor or in the world editor.
		else
		{
			// We can't use EditCategory like above, because of the peculiar issue.
			// Nested categories are named like "CategoryA|CategoryB|CategoryC", and EditCategory will generate
			// the category using full name, leading to the categories duplications.
			// To omit this problem it will work directly on the edited object properties, without categories, 
			// finding it handle and adding the preview widget to it.
			TArray<TWeakObjectPtr<UObject>> Objects;
			DetailLayout.GetObjectsBeingCustomized(Objects);
			for (TWeakObjectPtr<UObject> Obj : Objects)
			{
				if (Obj.IsValid() == false)
				{
					continue;
				}

				for (TFieldIterator<FProperty> It(Obj->GetClass()); It; ++It)
				{
					FProperty* Property = *It;
					if (Property->IsA<FTextProperty>() == false)
					{
						continue;
					}

					TSharedRef<IPropertyHandle> PropHandle = DetailLayout.GetProperty(Property->GetFName(), Obj->GetClass());
					if (PropHandle->IsValidHandle() == false)
					{
						continue;
					}

					IDetailPropertyRow* PropertyRow = DetailLayout.EditDefaultProperty(PropHandle);
					if (PropertyRow == nullptr)
					{
						continue;
					}

					// Get the default name and value widgets for the property row, so we can reuse them in the custom widget.
					TSharedPtr<SWidget> OutNameWidget;
					TSharedPtr<SWidget> OutValueWidget;
					PropertyRow->GetDefaultWidgets(OutNameWidget, OutValueWidget);

					PropertyRow->CustomWidget()
						.NameContent()
						[
							// Name the same as the original
							OutNameWidget->AsShared()
						]
						.ValueContent()
						[
							// Value the same as the original, but wrapped in the vertical box 
							// with the text block with the text preview below.
							SNew(SVerticalBox)
								+SVerticalBox::Slot()
								.AutoHeight()
								.Padding(0.f, 6.f)
								[
									OutValueWidget->AsShared()
								]
								+SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(STextBlock).Text_Lambda([this, PropHandle]
									{
										FText TextValue;
										PropHandle->GetValue(TextValue);
										return TextValue;
									})
									.Margin(FMargin(0.f, 0.f, 0.f, 6.f))
								]
						];
				}
			}				
		}
	}
}

TSharedPtr<SGraphPin> FTextPreviewGraphPanelPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	if (UELTEditorSettings::GetPreviewInUIEnabled() == false)
	{
		return nullptr;
	}

	if (Pin == nullptr)
	{
		return nullptr;
	}

	const FEdGraphPinType& PinType = Pin->PinType;
	if (PinType.PinCategory == UEdGraphSchema_K2::PC_Text)
	{
		return SNew(SGraphPinTextPreview, Pin);
	}

	return nullptr;
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION

#endif // ELTEDITOR_WITH_PREVIEW_IN_UI