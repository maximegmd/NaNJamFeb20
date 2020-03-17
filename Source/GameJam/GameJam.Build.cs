// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameJam : ModuleRules
{
	public GameJam(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "Steamworks", "Networking", "OnlineSubsystemUtils" });
        PrivateDependencyModuleNames.AddRange(new string[] { "HeadMountedDisplay", "AIModule", "UMG", "OnlineSubsystem", "OnlineSubsystemUtils" }) ;

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
