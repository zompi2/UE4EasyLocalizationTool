// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/**
 * Class handling UICommands of the editor.
 * Currently only "Open ELT Window" commands is required.
 * It is done via commands, because we want to have a keyboard shortcut for it.
 */

class EASYLOCALIZATIONTOOLEDITOR_API FELTEditorCommands : public TCommands<FELTEditorCommands>
{
public:

	FELTEditorCommands();

	void RegisterCommands() override;
	
	TSharedPtr<FUICommandInfo> OpenELTMenu;
};
