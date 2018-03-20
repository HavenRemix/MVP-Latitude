// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class CoopGame : ModuleRules
{
	public CoopGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Slate", "SlateCore", "UMG", "OnlineSubsystem", "OnlineSubsystemSteam" });
	}
}
