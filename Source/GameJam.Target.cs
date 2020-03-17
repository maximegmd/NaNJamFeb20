// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GameJamTarget : TargetRules
{
	public GameJamTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
        bUsesSteam = true;
        ExtraModuleNames.AddRange( new string[] { "GameJam" } );
	}
}
