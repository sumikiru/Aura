// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Aura : ModuleRules
{
	public Aura(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTasks", "GameplayTags", "AIModule", "Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new[] { "NavigationSystem", "Niagara" });
	}
}