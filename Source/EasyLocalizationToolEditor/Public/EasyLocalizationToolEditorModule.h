// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Modules/ModuleManager.h"

/**
 * Localization Editor module, which handles Editor object and DockTab creation.
 */

class EASYLOCALIZATIONTOOLEDITOR_API FEasyLocalizationToolEditorModule : public IModuleInterface, public FGCObject
{

public:

	// IModuleInterface implementation
	void StartupModule() override;

	// FGCObject implementation
	void AddReferencedObjects(FReferenceCollector& Collector) override;

protected:

	/**
	 * Run some initializations after the Engine has been initialized.
	 */
	void OnPostEngineInit();

private:

	/**
	 * Returns true if the editor can be spawned.
	 */
	bool CanSpawnEditor();

	/**
	 * Spawns editor and returns a ref of the DockTab to which the editor
	 * has been pinned.
	 */
	TSharedRef<class SDockTab> SpawnEditor(const FSpawnTabArgs& Args);

	/**
	 * Checks if the editor is spawned.
	 */
	bool IsEditorSpawned();

	/**
	 * Invokes spawning editor from the command.
	 */
	void InvokeEditorSpawn();
	
	// Editor object.
	class UELTEditor* Editor;

	// DockTab reference with the editor.
	TWeakPtr<class SDockTab> EditorTab;
};
