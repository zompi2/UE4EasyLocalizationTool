// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

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
				"EditorStyle",
				"Projects"
			}
		);

		// Enable this option to use build in Unreal Engine CSV Parser instead of the one from this plugin
		PublicDefinitions.Add("ELT_USE_UNREAL_CSV_PARSER=0");

        // Ensure there are no duplicated definitions already
        PublicDefinitions.RemoveAll(ECFDefinition => ECFDefinition.StartsWith("ELTEDITOR_"));

        // Disable optimization for non shipping builds (for easier debugging)
        bool bDisableOptimization = false;
        if (bDisableOptimization && (Target.Configuration != UnrealTargetConfiguration.Shipping))
        {
            PublicDefinitions.Add("ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION=PRAGMA_DISABLE_OPTIMIZATION");
            PublicDefinitions.Add("ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION=PRAGMA_ENABLE_OPTIMIZATION");
        }
        else
        {
            PublicDefinitions.Add("ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION=");
            PublicDefinitions.Add("ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION=");
        }
    }
}
