// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#include "EasyLocalizationToolEditorModule.h"
#include "ELTEditor.h"
#include "ELTEditorCommands.h"

#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "Interfaces/IMainFrameModule.h"

#include "LevelEditor.h"

IMPLEMENT_MODULE(FEasyLocalizationToolEditorModule, EasyLocalizationToolEditor)

// Id of the ELT Tab used to spawn and observe this tab.
const FName ELTTabId = FName(TEXT("ELT"));

void FEasyLocalizationToolEditorModule::StartupModule()
{
	// Register UICommands.
	FELTEditorCommands::Register();

	// Register OnPostEngineInit delegate.
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FEasyLocalizationToolEditorModule::OnPostEngineInit);

	// Create and initialize Editor object.
	Editor = NewObject<UELTEditor>();
	Editor->Init();

	// Register Tab Spawner. Do not show it in menu, as it will be invoked manually by a UICommand.
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		ELTTabId,
		FOnSpawnTab::CreateRaw(this, &FEasyLocalizationToolEditorModule::SpawnEditor),
		FCanSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args) -> bool { return CanSpawnEditor(); })
	).SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FEasyLocalizationToolEditorModule::OnPostEngineInit()
{
	// This function is for registering UICommand to the engine, so it can be executed via keyboard shortcut.
	// This will also add this UICommand to the menu, so it can also be executed from there.
	
	// This function is valid only if no Commandlet or game is running. It also requires Slate Application to be initialized.
	if ((IsRunningCommandlet() == false) && (IsRunningGame() == false) && FSlateApplication::IsInitialized())
	{
		if (FLevelEditorModule* LevelEditor = FModuleManager::LoadModulePtr<FLevelEditorModule>(TEXT("LevelEditor")))
		{
			// Create a UICommandList and map editor spawning function to the UICommand of opening ELT Editor.
			TSharedPtr<FUICommandList> Commands = MakeShareable(new FUICommandList());
			Commands->MapAction(
				FELTEditorCommands::Get().OpenELTMenu,
				FExecuteAction::CreateRaw(this, &FEasyLocalizationToolEditorModule::InvokeEditorSpawn),
				FCanExecuteAction::CreateRaw(this, &FEasyLocalizationToolEditorModule::CanSpawnEditor),
				FIsActionChecked::CreateRaw(this, &FEasyLocalizationToolEditorModule::IsEditorSpawned)
			);

			// Register this UICommandList to the MainFrame.
			// Otherwise nothing will handle the input to trigger this command.
			IMainFrameModule& MainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
			MainFrame.GetMainFrameCommandBindings()->Append(Commands.ToSharedRef());

			// Create a Menu Extender, which adds a button that executes the UICommandList of opening ELT Window.
			TSharedPtr<FExtender> MainMenuExtender = MakeShareable(new FExtender);
			MainMenuExtender->AddMenuExtension(
				FName(TEXT("General")),
				EExtensionHook::After, 
				Commands,
				FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
				{
					MenuBuilder.AddMenuEntry(
						FELTEditorCommands::Get().OpenELTMenu,
						NAME_None,
						FText::FromString(TEXT("Easy Localization Tool")),
						FText::FromString(TEXT("Opens Easy Localization Tool Window")),
						FSlateIcon()
					);
				})
			);

			// Extend Editors menu with the created Menu Extender.
			LevelEditor->GetMenuExtensibilityManager()->AddExtender(MainMenuExtender);
		}
	}
}

void FEasyLocalizationToolEditorModule::AddReferencedObjects(FReferenceCollector& Collector)
{
	// Prevent Editor Object from being garbage collected.
	if (Editor)
	{
		Collector.AddReferencedObject(Editor);
	}
}

bool FEasyLocalizationToolEditorModule::CanSpawnEditor()
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
	if (CanSpawnEditor())
	{
		// Spawn new DockTab and fill it with newly created editor UI.
		TSharedRef<SDockTab> NewTab = SAssignNew(EditorTab, SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				Editor->CreateEditorUI()
			];

		// Tell the Editor Object about newly spawned DockTab, as it will 
		// need it to handle various editor actions.
		Editor->SetEditorTab(NewTab);

		// Return the DockTab to the Global Tab Manager.
		return NewTab;
	}

	// If editor can't be spawned - create an empty tab.
	return SAssignNew(EditorTab, SDockTab).TabRole(ETabRole::NomadTab);
}

bool FEasyLocalizationToolEditorModule::IsEditorSpawned()
{
	return FGlobalTabmanager::Get()->FindExistingLiveTab(ELTTabId).IsValid();
}

void FEasyLocalizationToolEditorModule::InvokeEditorSpawn()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ELTTabId);
}
