// Copyright (c) 2026 Damian Nowakowski. All rights reserved.

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
				"Projects",
				"GraphEditor",
				"EditorWidgets",
				"BlueprintGraph"
			}
		);

		if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 4)
		{
			PrivateDependencyModuleNames.Add("ToolMenus");
		}

		// Ensure there are no duplicated definitions already
		PublicDefinitions.RemoveAll(ECFDefinition => ECFDefinition.StartsWith("ELTEDITOR_"));

		// Disable optimization (for easier debugging)
		bool bDisableOptimization = true;
		if (bDisableOptimization)
		{
			if (Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 2)
			{
				PublicDefinitions.Add("ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION=UE_DISABLE_OPTIMIZATION");
				PublicDefinitions.Add("ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION=UE_ENABLE_OPTIMIZATION");
			}
			else
			{
				PublicDefinitions.Add("ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION=PRAGMA_DISABLE_OPTIMIZATION");
				PublicDefinitions.Add("ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION=PRAGMA_ENABLE_OPTIMIZATION");
			}
		}
		else
		{
			PublicDefinitions.Add("ELTEDITOR_PRAGMA_DISABLE_OPTIMIZATION=");
			PublicDefinitions.Add("ELTEDITOR_PRAGMA_ENABLE_OPTIMIZATION=");
		}

		bool bAddPreviewInUI = true;
		if (bAddPreviewInUI && Target.Version.MajorVersion == 5 && Target.Version.MinorVersion >= 5)
		{
			PublicDefinitions.Add("ELTEDITOR_WITH_PREVIEW_IN_UI=1");
		}
		else
		{
            PublicDefinitions.Add("ELTEDITOR_WITH_PREVIEW_IN_UI=0");
        }
	}
}
