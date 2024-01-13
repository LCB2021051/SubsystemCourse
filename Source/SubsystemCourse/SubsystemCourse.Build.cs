// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SubsystemCourse : ModuleRules
{
	public SubsystemCourse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput","OnlineSubSystem","OnlineSubSystemSteam" });
	}
}
