// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#include "EasyLocalizationToolEditor.h"
#include "WorkspaceMenuStructureModule.h"
#include "WorkspaceMenuStructure.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "ELTEditor.h"
#include "LevelEditor.h"

IMPLEMENT_MODULE(FEasyLocalizationToolEditorModule, EasyLocalizationToolEditor)

void FEasyLocalizationToolEditorModule::StartupModule()
{
	// Create and initialize Editor object
	Editor = NewObject<UELTEditor>();
	Editor->Init();

	// Register Tab Spawner in the Tools menu.
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName(TEXT("ELT")), FOnSpawnTab::CreateRaw(this, &FEasyLocalizationToolEditorModule::SpawnEditor), FCanSpawnTab::CreateRaw(this, &FEasyLocalizationToolEditorModule::CanSpawnEditor))
		.SetDisplayName(FText::FromString(TEXT("Easy Localization Tool")))
		.SetTooltipText(FText::FromString(TEXT("Open Easy Localization Tool Editor")))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
}

void FEasyLocalizationToolEditorModule::AddReferencedObjects(FReferenceCollector& Collector)
{
	// Prevent Editor Object from being garbage collected.
	if (Editor)
	{
		Collector.AddReferencedObject(Editor);
	}
}

bool FEasyLocalizationToolEditorModule::CanSpawnEditor(const FSpawnTabArgs& Args)
{
	// Editor can be spawned only when the Editor object say that UI can be created.
	if (Editor && Editor->CanCreateEditorUI())
	{
		return true;
	}
	return false;
}

TSharedRef<SDockTab> FEasyLocalizationToolEditorModule::SpawnEditor(const FSpawnTabArgs& Args)
{	
	// Spawn the Editor only when we can.
	if (CanSpawnEditor(Args))
	{
		// Spawn new DockTab and fill it with newly created editor UI.
		TSharedRef<SDockTab> NewTab = SAssignNew(EditorTab, SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				Editor->CreateEditorUI()
			];

		// Tell the Editor Object about newly spawned DockTab, as it will 
		// need it to handle various editor actions.
		Editor->EditorTab = NewTab;

		// Return the DockTab to the Global Tab Manager.
		return NewTab;
	}

	// If editor can't be spawned - create an empty tab.
	return SAssignNew(EditorTab, SDockTab).TabRole(ETabRole::NomadTab);
}
