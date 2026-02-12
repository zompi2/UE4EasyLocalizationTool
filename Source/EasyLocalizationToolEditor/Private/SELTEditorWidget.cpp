// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "SELTEditorWidget.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

void SELTEditorWidget::Construct(const FArguments& InArgs)
{
	SUserWidget::Construct(SUserWidget::FArguments()
	[
		SNew(STextBlock)
		.Text(INVTEXT("My Widget's Content"))
	]);
}

TSharedRef<SELTEditorWidget> SELTEditorWidget::New()
{
	return MakeShareable(new SELTEditorWidget());
}

void SELTEditorWidget::FillLocalizationPaths(const TArray<FString>& Paths)
{
	
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION