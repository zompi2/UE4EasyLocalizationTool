// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SUserWidget.h"

class UELTEditorWidget;

class EASYLOCALIZATIONTOOLEDITOR_API SELTEditorWidget : public SUserWidget
{

public:

	SLATE_BEGIN_ARGS(SELTEditorWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	static TSharedRef<SELTEditorWidget> New();

	void FillLocalizationPaths(const TArray<FString>& Paths);

	UELTEditorWidget* WidgetController = nullptr;
};