// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GearOfThree : ModuleRules
{
	public GearOfThree(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"GearOfThree",
			"GearOfThree/Variant_Platforming",
			"GearOfThree/Variant_Platforming/Animation",
			"GearOfThree/Variant_Combat",
			"GearOfThree/Variant_Combat/AI",
			"GearOfThree/Variant_Combat/Animation",
			"GearOfThree/Variant_Combat/Gameplay",
			"GearOfThree/Variant_Combat/Interfaces",
			"GearOfThree/Variant_Combat/UI",
			"GearOfThree/Variant_SideScrolling",
			"GearOfThree/Variant_SideScrolling/AI",
			"GearOfThree/Variant_SideScrolling/Gameplay",
			"GearOfThree/Variant_SideScrolling/Interfaces",
			"GearOfThree/Variant_SideScrolling/UI",
			"GearOfThree/EnemyNPC/Public"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
