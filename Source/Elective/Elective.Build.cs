// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Elective : ModuleRules
{
	public Elective(ReadOnlyTargetRules Target) : base(Target)
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
			"Elective",
			"Elective/Variant_Platforming",
			"Elective/Variant_Platforming/Animation",
			"Elective/Variant_Combat",
			"Elective/Variant_Combat/AI",
			"Elective/Variant_Combat/Animation",
			"Elective/Variant_Combat/Gameplay",
			"Elective/Variant_Combat/Interfaces",
			"Elective/Variant_Combat/UI",
			"Elective/Variant_SideScrolling",
			"Elective/Variant_SideScrolling/AI",
			"Elective/Variant_SideScrolling/Gameplay",
			"Elective/Variant_SideScrolling/Interfaces",
			"Elective/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
