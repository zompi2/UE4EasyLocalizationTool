// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class EasyLocalizationToolEditor : ModuleRules
{
	public EasyLocalizationToolEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateIncludePaths.Add("EasyLocalizationToolEditor/Private");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EasyLocalizationTool",
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
