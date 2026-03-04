// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "LocTextDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "LocText.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"
#include "ELT.h"
#include "Runtime/Launch/Resources/Version.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

TSharedRef<IPropertyTypeCustomization> FLocTextDetails::MakeInstance()
{
	return MakeShared<FLocTextDetails>();
}

void FLocTextDetails::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	NamespaceProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocText, Namespace));
	KeyProperty = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLocText, Key));

	ValueResult.Bind(this, &FLocTextDetails::GetText);
}

void FLocTextDetails::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (NamespaceProperty && KeyProperty)
	{	
		IDetailGroup& StructGroup = StructBuilder.AddGroup("LocText", StructPropertyHandle->GetPropertyDisplayName());
		StructGroup.AddPropertyRow(NamespaceProperty.ToSharedRef());
		StructGroup.AddPropertyRow(KeyProperty.ToSharedRef());
		StructGroup.AddWidgetRow()
		.NameContent()
		[
			SNew(SBox)
			.Content()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Value"))
			]
		]
		.ValueContent()
		[
			SNew(SBox)
			.Content()
			[
				SNew(STextBlock)
				.Text(ValueResult)
			]
		];
	}
}

FText FLocTextDetails::GetText() const
{
	if (NamespaceProperty && KeyProperty)
	{
		FString NamespaceValue, KeyValue;
		NamespaceProperty->GetValue(NamespaceValue);
		KeyProperty->GetValue(KeyValue);

		return UELT::GetLocalizedText(NamespaceValue, KeyValue);
	}
	
	return FText::GetEmpty();
}


ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION