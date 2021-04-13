// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#include "EasyLocalizationToolEditor.h"
#include "WorkspaceMenuStructureModule.h"
#include "WorkspaceMenuStructure.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "ELTEditor.h"
#include "LevelEditor.h"

void FEasyLocalizationToolEditorModule::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (Editor)
	{
		Collector.AddReferencedObject(Editor);
	}
}

void FEasyLocalizationToolEditorModule::StartupModule()
{
	Editor = NewObject<UELTEditor>();
	Editor->Init();

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName(TEXT("ELT")), FOnSpawnTab::CreateRaw(this, &FEasyLocalizationToolEditorModule::SpawnEditor), FCanSpawnTab::CreateRaw(this, &FEasyLocalizationToolEditorModule::CanSpawnEditor))
		.SetDisplayName(FText::FromString(TEXT("Easy Localization Tool")))
		.SetTooltipText(FText::FromString(TEXT("Open Easy Localization Tool Editor")))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());
}

bool FEasyLocalizationToolEditorModule::CanSpawnEditor(const FSpawnTabArgs& Args)
{
	if (Editor && Editor->CanCreateEditorUI())
	{
		return true;
	}
	return false;
}

TSharedRef<SDockTab> FEasyLocalizationToolEditorModule::SpawnEditor(const FSpawnTabArgs& Args)
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

IMPLEMENT_MODULE(FEasyLocalizationToolEditorModule, EasyLocalizationToolEditor)