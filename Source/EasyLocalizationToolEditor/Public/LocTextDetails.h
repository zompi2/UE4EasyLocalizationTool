// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

/**
 * Definition of LocText property Details Panel.
 * We override a default panel in order to conveniently display a localized text using
 * provided namespace and key in this property.
 */

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

class EASYLOCALIZATIONTOOLEDITOR_API FLocTextDetails : public IPropertyTypeCustomization
{
public:

	/** Makes a new instance of this detail layout property for a specific detail view requesting it. */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface. */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils & StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	/** */

private:

	/**
	 * Gets a localized text based on a key and namespace.
	 */
	FText GetText() const;

	/** Handling pointers to the namespace and key property. */
	TSharedPtr<IPropertyHandle> NamespaceProperty;
	TSharedPtr<IPropertyHandle> KeyProperty;

	/** Attribute holding localized text used to display it on details panel. */
	TAttribute<FText> ValueResult;
};
