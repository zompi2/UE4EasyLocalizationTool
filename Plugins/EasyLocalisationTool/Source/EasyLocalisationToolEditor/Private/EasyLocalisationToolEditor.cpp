// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#include "EasyLocalisationToolEditor.h"
#include "WorkspaceMenuStructureModule.h"
#include "WorkspaceMenuStructure.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "ELTEditor.h"
#include "LevelEditor.h"

void FEasyLocalisationToolEditorModule::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (Editor)
	{
		Collector.AddReferencedObject(Editor);
	}
}

void FEasyLocalisationToolEditorModule::StartupModule()
{
	Editor = NewObject<UELTEditor>();
	Editor->Init();

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName(TEXT("ELT")), FOnSpawnTab::CreateRaw(this, &FEasyLocalisationToolEditorModule::SpawnEditor), FCanSpawnTab::CreateRaw(this, &FEasyLocalisationToolEditorModule::CanSpawnEditor))
		.SetDisplayName(FText::FromString(TEXT("Easy Localisation Tool")))
		.SetTooltipText(FText::FromString(TEXT("Open Easy Localisation Tool Editor")))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
}

bool FEasyLocalisationToolEditorModule::CanSpawnEditor(const FSpawnTabArgs& Args)
{
	if (Editor && Editor->CanCreateEditorUI())
	{
		return true;
	}
	return false;
}

TSharedRef<SDockTab> FEasyLocalisationToolEditorModule::SpawnEditor(const FSpawnTabArgs& Args)
{	
	if (Editor && Editor->CanCreateEditorUI())
	{
		TSharedRef<SDockTab> NewTab = SAssignNew(EditorTab, SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				Editor->CreateEditorUI()
			];
		Editor->EditorTab = NewTab;
		return NewTab;
	}
	return SAssignNew(EditorTab, SDockTab).TabRole(ETabRole::NomadTab);
}

IMPLEMENT_MODULE(FEasyLocalisationToolEditorModule, EasyLocalisationToolEditor)