// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class GameJamEditorTarget : TargetRules
{
	public GameJamEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
        bUsesSteam = true;
		ExtraModuleNames.AddRange( new string[] { "GameJam" } );
	}
}
