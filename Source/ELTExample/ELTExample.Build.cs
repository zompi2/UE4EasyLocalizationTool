// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

using UnrealBuildTool;

public class ELTExample : ModuleRules
{
	public ELTExample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"UMG",
			"EasyLocalizationTool"
		});
	}
}
