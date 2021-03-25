// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class EasyLocalisationToolEditor : ModuleRules
{
	public EasyLocalisationToolEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateIncludePaths.Add("EasyLocalisationToolEditor/Private");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EasyLocalisationTool",
				"Blutility",
				"UMG",
				"UMGEditor"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"WorkspaceMenuStructure",
				"DesktopPlatform",
				"LevelEditor"
			}
		);
	}
}
