// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

/**
 * Because the SGraphPinText is private, we have no other option but
 * copy this class and add our changes to provide preview under the 
 * FText pins in BP nodes.
 * If at any point SGraphPinText become public, it should be rewritten
 * to inherit from the SGraphPinText.
 */

#pragma once

#if ELTEDITOR_WITH_PREVIEW_IN_UI

#include "SGraphPin.h"
#include "Templates/SharedPointer.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SWidget;
class UEdGraphPin;

class EASYLOCALIZATIONTOOLEDITOR_API SGraphPinTextPreview : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGraphPinTextPreview) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:
	//~ Begin SGraphPin Interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	//~ End SGraphPin Interface
};

#endif