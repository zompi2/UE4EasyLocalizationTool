// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#include "ELTEditorCommands.h"
#include "EditorStyleSet.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

FELTEditorCommands::FELTEditorCommands() : 
	TCommands<FELTEditorCommands>(
		TEXT("Easy Localization Tool Commands"), 
		FText::FromString(TEXT("Commands to control Easy Localization Tool")), 
		NAME_None, 
		FEditorStyle::GetStyleSetName()
	)
{}

void FELTEditorCommands::RegisterCommands()
{
#define LOCTEXT_NAMESPACE "ELTLoc"
	UI_COMMAND(OpenELTMenu, "Easy Localization Tool", "Opens Easy Localisation Tool Editor Window", EUserInterfaceActionType::Check, FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::L));
#undef LOCTEXT_NAMESPACE
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION