// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class EasyLocalizationTool : ModuleRules
{
	public EasyLocalizationTool(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.Add("EasyLocalizationTool/Private");

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
			}
		);

        // Ensure there are no duplicated definitions already
        PublicDefinitions.RemoveAll(ECFDefinition => ECFDefinition.StartsWith("ELT_"));

        // Disable optimization for non shipping builds (for easier debugging)
        bool bDisableOptimization = false;
        if (bDisableOptimization && (Target.Configuration != UnrealTargetConfiguration.Shipping))
        {
            PublicDefinitions.Add("ELT_PRAGMA_DISABLE_OPTIMIZATION=PRAGMA_DISABLE_OPTIMIZATION");
            PublicDefinitions.Add("ELT_PRAGMA_ENABLE_OPTIMIZATION=PRAGMA_ENABLE_OPTIMIZATION");
        }
        else
        {
            PublicDefinitions.Add("ELT_PRAGMA_DISABLE_OPTIMIZATION=");
            PublicDefinitions.Add("ELT_PRAGMA_ENABLE_OPTIMIZATION=");
        }
    }
}
