// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#include "TextLocPreview.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "IDetailGroup.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"

FTextLocPreview::FTextLocPreview(UBlueprint* InBlueprintPtr)
	: BlueprintPtr(InBlueprintPtr)
{
}

FTextLocPreview::FTextLocPreview()
{
}

TSharedRef<IDetailCustomization> FTextLocPreview::MakeInstance()
{
	return MakeShareable(new FTextLocPreview);
}

TSharedPtr<IDetailCustomization> FTextLocPreview::MakeInstanceBP(TSharedPtr<IBlueprintEditor> InBlueprintEditor)
{
	if (InBlueprintEditor.IsValid() == false)
	{
		return nullptr;
	}

	const TArray<UObject*>* Objects = InBlueprintEditor->GetObjectsCurrentlyBeingEdited();
	if (Objects == nullptr || Objects->Num() != 1)
	{
		return nullptr;
	}

	UBlueprint* Blueprint = Cast<UBlueprint>((*Objects)[0]);
	if (Blueprint == nullptr)
	{
		return nullptr;
	}

	return MakeShareable(new FTextLocPreview(Blueprint));
}

UE_DISABLE_OPTIMIZATION

// IDetailCustomization interface
void FTextLocPreview::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailLayout.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	if (ObjectsBeingCustomized.Num() != 1)
	{
		return;
	}
	
	TArray<FName> CategoryNames;
	DetailLayout.GetCategoryNames(CategoryNames);
	for (const FName& CatName : CategoryNames)
	{
		IDetailCategoryBuilder& Category = DetailLayout.EditCategory(CatName);

		TArray<TSharedRef<IPropertyHandle>> PropertyHandles;
		Category.GetDefaultProperties(PropertyHandles);
		for (TSharedRef<IPropertyHandle>& Prop : PropertyHandles)
		{
			if (Prop->IsValidHandle())
			{
				FText TextPropValue;
				if (Prop->GetValue(TextPropValue) == FPropertyAccess::Success)
				{
					if (CatName == "DefaultValueCategory")
					{
						DetailLayout.EditCategory("DefaultValueCategory")
							.AddCustomRow(INVTEXT("Preview"))
							.NameContent()
							[
								SNew(SHorizontalBox)
									+ SHorizontalBox::Slot()
									.AutoWidth()
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(STextBlock).Text(INVTEXT("Preview"))
									]
							]
						.ValueContent()
							[
								SNew(SHorizontalBox)
									+ SHorizontalBox::Slot()
									.AutoWidth()
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									[
										SNew(STextBlock).Text(TextPropValue)
									]
							];
					}
					else
					{
						IDetailPropertyRow* PropertyRow = DetailLayout.EditDefaultProperty(Prop);
						if (PropertyRow)
						{
							//	PropertyRow->GetWidgetRow();
							TSharedPtr<SWidget> OutNameWidget;
							TSharedPtr<SWidget> OutValueWidget;
							PropertyRow->GetDefaultWidgets(OutNameWidget, OutValueWidget);

							int z = 0;

							PropertyRow->CustomWidget()
								.NameContent()
								[
									OutNameWidget->AsShared()
								]
								.ValueContent()
								[
									SNew(SVerticalBox)
										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											OutValueWidget->AsShared()
										]
										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											SNew(STextBlock).Text(TextPropValue)
										]
								];
						}
					}		
				}
			}
		}
	}
}

UE_ENABLE_OPTIMIZATION