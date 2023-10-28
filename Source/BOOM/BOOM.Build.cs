// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BOOM : ModuleRules
{
	public BOOM(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay"});

        PublicDependencyModuleNames.AddRange(new string[] {"AIModule", "EnhancedInput", "NavigationSystem", "GameplayAbilities", "GameplayTags", "GameplayTasks" });
    }
}
