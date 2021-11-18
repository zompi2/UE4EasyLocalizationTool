// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class EasyLocalizationToolEditor : ModuleRules
{
	public EasyLocalizationToolEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("EasyLocalizationToolEditor/Private");
		PrivateIncludePaths.Add("EasyLocalizationTool/Private");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"InputCore"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"EasyLocalizationTool",
                "Engine",
				"CoreUObject",
                "Slate",
                "SlateCore",
                "UnrealEd",
				"WorkspaceMenuStructure",
				"DesktopPlatform",
                "Blutility",
                "UMG",
                "UMGEditor",
				"EditorStyle"
			}
		);

		PublicDefinitions.Add("ELT_USE_UNREAL_CSV_PARSER=0");
	}
}
