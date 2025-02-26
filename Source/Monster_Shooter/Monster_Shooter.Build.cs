// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Monster_Shooter : ModuleRules
{
	public Monster_Shooter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay","UMG","AIModule" });
	}
}
