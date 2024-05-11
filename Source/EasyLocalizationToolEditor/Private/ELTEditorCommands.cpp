// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#include "ELTEditorCommands.h"
#include "EditorStyleSet.h"

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

FELTEditorCommands::FELTEditorCommands() : 
	TCommands<FELTEditorCommands>(
		TEXT("Easy Localization Tool Commands"), 
		FText::FromString(TEXT("Commands to control Easy Localization Tool")), 
		NAME_None, 
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 1))
		FAppStyle::GetAppStyleSetName()
#else
		FEditorStyle::GetStyleSetName()
#endif
	)
{}

void FELTEditorCommands::RegisterCommands()
{
#define LOCTEXT_NAMESPACE "ELTLoc"
	UI_COMMAND(OpenELTMenu, "Easy Localization Tool", "Opens Easy Localisation Tool Editor Window", EUserInterfaceActionType::Check, FInputChord(EModifierKey::Shift | EModifierKey::Alt, EKeys::L));
#undef LOCTEXT_NAMESPACE
}

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION