// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

#include "EasyLocalizationToolEditorModule.h"
#include "ELTEditor.h"
#include "ELTEditorCommands.h"
#include "ELTEditorStyle.h"
#include "LocTextDetails.h"
#include "TextLocPreview.h"

#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "Interfaces/IMainFrameModule.h"

#include "PropertyEditorModule.h"
#include "LevelEditor.h"

#include "ContentBrowserModule.h"
#include "ContentBrowserDelegates.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ELTEditorAuditor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Editor.h"

#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 4))
#include "ToolMenu.h"
#include "Toolkits/AssetEditorToolkitMenuContext.h"
#include "Toolkits/AssetEditorToolkit.h"
#endif

IMPLEMENT_MODULE(FEasyLocalizationToolEditorModule, EasyLocalizationToolEditor)

ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION

// Id of the ELT Tab used to spawn and observe this tab.
const FName ELTTabId = FName(TEXT("ELT"));

void FEasyLocalizationToolEditorModule::StartupModule()
{
	// Register Styles.
	FELTEditorStyle::Initialize();
	FELTEditorStyle::ReloadTextures();

	// Register UICommands.
	FELTEditorCommands::Register();

	// Register OnPostEngineInit delegate.
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 8))
	OnPostEngineInitDelegateHandle = FCoreDelegates::GetOnPostEngineInit().AddRaw(this, &FEasyLocalizationToolEditorModule::OnPostEngineInit);
#else
	OnPostEngineInitDelegateHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FEasyLocalizationToolEditorModule::OnPostEngineInit);
#endif

	// Create and initialize Editor object.
	Editor = NewObject<UELTEditor>();
	Editor->Init();

	// Register Tab Spawner. Do not show it in menu, as it will be invoked manually by a UICommand.
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		ELTTabId,
		FOnSpawnTab::CreateRaw(this, &FEasyLocalizationToolEditorModule::SpawnEditor),
		FCanSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args) -> bool { return CanSpawnEditor(); })
	)
	.SetMenuType(ETabSpawnerMenuType::Hidden)
	.SetIcon(FSlateIcon(FELTEditorStyle::GetStyleSetName(), "ELTEditorStyle.MenuIcon"));

#if ELTEDITOR_WITH_PREVIEW_IN_UI
	// Register Text pin factory to show localization preview under Text pins in BP nodes.
	GraphPanelPinFactory = MakeShared<FTextPreviewGraphPanelPinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory(GraphPanelPinFactory);
#endif

	// Register the Localization Audit action in the Content Browser context menu.
	FContentBrowserMenuExtender_SelectedAssets AuditExtender = FContentBrowserMenuExtender_SelectedAssets::CreateLambda(
		[](const TArray<FAssetData>& SelectedAssets) -> TSharedRef<FExtender>
		{
			TSharedRef<FExtender> Extender = MakeShared<FExtender>();
			Extender->AddMenuExtension(
				"CommonAssetActions",
				EExtensionHook::After,
				nullptr,
				FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
				{
					MenuBuilder.BeginSection("ELTLocalizationAudit", FText::FromString("Easy Localization Tool"));
					MenuBuilder.AddMenuEntry(
						FText::FromString("Localization Audit"),
						FText::FromString("\
Scans asset(s) for all FText properties marked to be localize, and returns them in a list.\n\n\
They are tested for the following issues:\n\
1. Empty Text Value\n\
2. Invalid String Table Key Reference\n\
3. Not Yet Localized (Value and Localization Key do not match)\n\
4. Invalid Localization (Value and Localization Key matches, but does not return a valid localization string)"),
						FSlateIcon(),
						FUIAction(FExecuteAction::CreateLambda([]()
						{
							GetMutableDefault<UELTEditorAuditor>()->AuditSelectedAssets();
						}))
					);
					MenuBuilder.EndSection();
				})
			);
			return Extender;
		}
	);
	ContentBrowserExtenderHandle = AuditExtender.GetHandle();
	FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"))
		.GetAllAssetViewContextMenuExtenders().Add(MoveTemp(AuditExtender));

	FContentBrowserMenuExtender_SelectedPaths FolderAuditExtender = FContentBrowserMenuExtender_SelectedPaths::CreateLambda(
		[](const TArray<FString>& SelectedPaths) -> TSharedRef<FExtender>
		{
			TSharedRef<FExtender> Extender = MakeShared<FExtender>();
			Extender->AddMenuExtension(
				"PathViewFolderOptions",
				EExtensionHook::After,
				nullptr,
				FMenuExtensionDelegate::CreateLambda([SelectedPaths](FMenuBuilder& MenuBuilder)
				{
					MenuBuilder.BeginSection("ELTLocalizationAudit", FText::FromString("Easy Localization Tool"));
					MenuBuilder.AddMenuEntry(
						FText::FromString("Localization Audit"),
						FText::FromString("Scans all assets in the selected folder(s) for FText localization issues."),
						FSlateIcon(),
						FUIAction(FExecuteAction::CreateLambda([SelectedPaths]()
						{
							FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
							IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

							FARFilter Filter;
							Filter.bRecursivePaths = true;
							for (const FString& Path : SelectedPaths)
							{
								// GetAllPathViewContextMenuExtenders also returns /All-prefixed
								// virtual paths — strip to get the real package path.
								FString PackagePath = Path;
								const FString AllPrefix = TEXT("/All");
								if (PackagePath.StartsWith(AllPrefix))
								{
									PackagePath = PackagePath.RightChop(AllPrefix.Len());
								}
								Filter.PackagePaths.Add(FName(*PackagePath));
							}

							TArray<FAssetData> Assets;
							AssetRegistry.GetAssets(Filter, Assets);

							// Also include any assets selected in the asset view — the path view
							// extender fires when right-clicking a folder regardless of asset selection,
							// so we need to merge both selections manually.
							TArray<FAssetData> SelectedAssets;
							FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"))
								.Get().GetSelectedAssets(SelectedAssets);
							for (const FAssetData& SelectedAsset : SelectedAssets)
							{
								Assets.AddUnique(SelectedAsset);
							}

							GetMutableDefault<UELTEditorAuditor>()->AuditAssets(Assets);
						}))
					);
					MenuBuilder.EndSection();
				})
			);
			return Extender;
		}
	);
	PathViewExtenderHandle = FolderAuditExtender.GetHandle();
	FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"))
		.GetAllPathViewContextMenuExtenders().Add(MoveTemp(FolderAuditExtender));
}

void FEasyLocalizationToolEditorModule::ShutdownModule()
{
#if ELTEDITOR_WITH_PREVIEW_IN_UI
	// Unregister Text pin factory
	if (GraphPanelPinFactory)
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(GraphPanelPinFactory);
	}
#endif

	// Unregister LocText custom details panel.
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout("LocText");
#if USE_STABLE_LOCALIZATION_KEYS
	PropertyModule.UnregisterCustomClassLayout(UObject::StaticClass()->GetFName());
#endif

	// Unregister the Localization Audit content browser extender.
	if (FContentBrowserModule* CBModule = FModuleManager::GetModulePtr<FContentBrowserModule>(TEXT("ContentBrowser")))
	{
		CBModule->GetAllAssetViewContextMenuExtenders().RemoveAll(
			[this](const FContentBrowserMenuExtender_SelectedAssets& Delegate)
			{
				return Delegate.GetHandle() == ContentBrowserExtenderHandle;
			}
		);
		CBModule->GetAllPathViewContextMenuExtenders().RemoveAll(
			[this](const FContentBrowserMenuExtender_SelectedPaths& Delegate)
			{
				return Delegate.GetHandle() == PathViewExtenderHandle;
			}
		);
	}

	// Unregister Tab Spawner
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ELTTabId);

	// Cleanup the Editor object.
	Editor = nullptr;

	// Remove OnPostEngineInit delegate
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 8))
	FCoreDelegates::GetOnPostEngineInit().Remove(OnPostEngineInitDelegateHandle);
#else
	FCoreDelegates::OnPostEngineInit.Remove(OnPostEngineInitDelegateHandle);
#endif

	// Unregister UICommands.
	FELTEditorCommands::Unregister();

	// Unregister Styles.
	FELTEditorStyle::Shutdown();
}

void FEasyLocalizationToolEditorModule::RunAuditCommand()
{
	if (!FSlateApplication::IsInitialized() || !GEditor)
	{
		return;
	}

	TArray<FAssetData> AssetsToAudit;
	bool bContentBrowserHasFocus = false;

	// Check if the active window/widget is a Content Browser or Content Drawer
	TSharedPtr<SWidget> FocusedWidget = FSlateApplication::Get().GetUserFocusedWidget(0);
	if (FocusedWidget.IsValid())
	{
		FWidgetPath FocusedWidgetPath;
		if (FSlateApplication::Get().GeneratePathToWidgetUnchecked(FocusedWidget.ToSharedRef(), FocusedWidgetPath))
		{
			for (int32 i = FocusedWidgetPath.Widgets.Num() - 1; i >= 0; --i)
			{
				FString WidgetType = FocusedWidgetPath.Widgets[i].Widget->GetTypeAsString();

				// This catches active standalone content browsers and the pop-up content drawer
				if (WidgetType.Contains("SContentBrowser") || WidgetType.Contains("SContentDrawer"))
				{
					bContentBrowserHasFocus = true;
					break;
				}
			}
		}
	}

	// If bContentBrowserHasFocus is true, audit selected assets and folders. Does nothing if nothing is selected.
	if (bContentBrowserHasFocus)
	{
		GetMutableDefault<UELTEditorAuditor>()->AuditSelectedAssets();
		return;
	}

	// Check if the active window is an asset, audit it if true.
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (AssetEditorSubsystem == nullptr)
	{
		return;
	}
	
	// Loop through all opened assets and check if the asset window is current tab. 
	for (UObject* Asset : AssetEditorSubsystem->GetAllEditedAssets())
	{
		IAssetEditorInstance* EditorInstance = AssetEditorSubsystem->FindEditorForAsset(Asset, false);
		if (!EditorInstance)
		{
			continue;
		}

		FAssetEditorToolkit* EditorToolkit = static_cast<FAssetEditorToolkit*>(EditorInstance);
		if (!EditorToolkit)
		{
			continue;
		}

		TSharedPtr<FTabManager> TabManager = EditorToolkit->GetTabManager();
		if (!TabManager)
		{
			continue;
		}

		TSharedPtr<SDockTab> OwnerTab = TabManager->GetOwnerTab();
		if (OwnerTab.IsValid() && OwnerTab->IsForeground())
		{
			AssetsToAudit.Add(FAssetData(Asset));
			GetMutableDefault<UELTEditorAuditor>()->AuditAssets(AssetsToAudit);
			return;
		}
	}
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

			// Map the audit shortcut to RunAuditCommand.
			Commands->MapAction(
				FELTEditorCommands::Get().RunELTAudit,
				FExecuteAction::CreateRaw(this, &FEasyLocalizationToolEditorModule::RunAuditCommand)
			);

			// Register this UICommandList to the MainFrame.
			// Otherwise nothing will handle the input to trigger this command.
			IMainFrameModule& MainFrame = FModuleManager::Get().LoadModuleChecked<IMainFrameModule>("MainFrame");
			MainFrame.GetMainFrameCommandBindings()->Append(Commands.ToSharedRef());

			
#if ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 4))
			// Add both the ELT window opener and Localization Audit to the same named section
			// under the Tools menu, so they appear grouped as "Easy Localization Tool" entries.
			UToolMenu* ToolsMenu = UToolMenus::Get()->ExtendMenu(TEXT("MainFrame.MainMenu.Tools"));
			if (ToolsMenu)
			{
				FToolMenuSection& Section = ToolsMenu->FindOrAddSection(
					"ELTLocalizationAudit",
					FText::FromString(TEXT("Easy Localization Tool"))
				);

				Section.AddEntry(FToolMenuEntry::InitMenuEntryWithCommandList(
					FELTEditorCommands::Get().OpenELTMenu,
					Commands,
					FText::FromString(TEXT("Easy Localization Tool")),
					FText::FromString(TEXT("Opens Easy Localization Tool Window")),
					FSlateIcon(FELTEditorStyle::GetStyleSetName(), "ELTEditorStyle.MenuIcon")));

				Section.AddEntry(FToolMenuEntry::InitMenuEntryWithCommandList(
					FELTEditorCommands::Get().RunELTAudit,
					Commands,
					FText::FromString(TEXT("Localization Audit")),
					FText::FromString(TEXT("\
Scans this asset for all FText properties marked to be localize, and returns them in a list.\n\n\
They are tested for the following issues:\n\
1. Empty Text Value\n\
2. Invalid String Table Key Reference\n\
3. Not Yet Localized (Value and Localization Key do not match)\n\
4. Invalid Localization (Value and Localization Key matches, but does not return a valid localization string)")),
					FSlateIcon(FELTEditorStyle::GetStyleSetName(), "ELTEditorStyle.MenuIcon")));
			}
#else
			// Create a Menu Extender, which adds a button that executes the UICommandList of opening ELT Window.
			TSharedPtr<FExtender> MainMenuExtender = MakeShareable(new FExtender);
			MainMenuExtender->AddMenuExtension(
#if (ENGINE_MAJOR_VERSION == 5)
				FName(TEXT("Tools")),
#else
				FName(TEXT("General")),
#endif
				EExtensionHook::After, 
				Commands,
				FMenuExtensionDelegate::CreateLambda([](FMenuBuilder& MenuBuilder)
				{
					MenuBuilder.AddMenuEntry(
						FELTEditorCommands::Get().OpenELTMenu,
						NAME_None,
						FText::FromString(TEXT("Easy Localization Tool")),
						FText::FromString(TEXT("Opens Easy Localization Tool Window")),
						FSlateIcon(FELTEditorStyle::GetStyleSetName(), "ELTEditorStyle.MenuIcon")
					);
				})
			);

			// Extend Editors menu with the created Menu Extender.
			LevelEditor->GetMenuExtensibilityManager()->AddExtender(MainMenuExtender);
#endif
		}

		// Register LocText custom details panel and expand the FText details panel with localization preview.
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyModule.RegisterCustomPropertyTypeLayout("LocText", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FLocTextDetails::MakeInstance));
#if ELTEDITOR_WITH_PREVIEW_IN_UI
		// This will add a preview of the localized text under any FText property in BP editor and details panel.
		// It has to overwrite whole UObject class layout, instead of the property type layout, because the built-in property types
		// like FText can't be overriden. 
		PropertyModule.RegisterCustomClassLayout(UObject::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FTextLocPreview::MakeInstance));
#endif
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

#if (ENGINE_MAJOR_VERSION == 5)
FString FEasyLocalizationToolEditorModule::GetReferencerName() const
{
	return TEXT("ELTEditorModuleGCObject");
}
#endif

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

ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION