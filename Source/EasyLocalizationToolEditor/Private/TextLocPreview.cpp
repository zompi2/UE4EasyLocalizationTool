// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 5)) 

#include "TextLocPreview.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ELTEditorSettings.h"

TSharedRef<IDetailCustomization> FTextLocPreview::MakeInstance()
{
	return MakeShareable(new FTextLocPreview);
}

void FTextLocPreview::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	if (UELTEditorSettings::GetPreviewInUIEnabled() == false)
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
						DetailLayout.EditCategory(CatName)
							.AddCustomRow(INVTEXT("LocPreview"))
							.ValueContent()
							[
								SNew(SHorizontalBox)
									+ SHorizontalBox::Slot()
									.AutoWidth()
									[
										SNew(STextBlock).Text_Lambda([this, Prop]
										{
											FText TextValue;
											Prop->GetValue(TextValue);
											return TextValue;
										})
									]
							];
					}
					else
					{
						IDetailPropertyRow* PropertyRow = DetailLayout.EditDefaultProperty(Prop);
						if (PropertyRow)
						{
							TSharedPtr<SWidget> OutNameWidget;
							TSharedPtr<SWidget> OutValueWidget;
							PropertyRow->GetDefaultWidgets(OutNameWidget, OutValueWidget);

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
										.Padding(0.f, 6.f)
										[
											OutValueWidget->AsShared()
										]
										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											SNew(STextBlock).Text_Lambda([this, Prop]
											{
												FText TextValue;
												Prop->GetValue(TextValue);
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
	}
}

#endif // ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 5))