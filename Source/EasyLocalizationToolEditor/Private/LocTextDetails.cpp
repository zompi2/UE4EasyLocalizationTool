// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#include "LocTextDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "LocText.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailGroup.h"
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

		const FTextId TextId(*NamespaceValue, *KeyValue);
		FText OutText;
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 5))
		FText::FindTextInLiveTable_Advanced(TextId.GetNamespace(), TextId.GetKey(), OutText, &KeyValue);
#else
		FText::FindText(TextId.GetNamespace(), TextId.GetKey(), OutText, &KeyValue);
#endif

		return OutText;
	}
	
	return FText::GetEmpty();
}


ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION