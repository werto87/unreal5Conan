// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

public class MyProject3 : ModuleRules
{
	public MyProject3(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] {"MyBoilerplateModule", "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
        Type type = Target.GetType();
        FieldInfo innerField = type.GetField("Inner", BindingFlags.Instance | BindingFlags.NonPublic);
        TargetRules inner = (TargetRules)innerField.GetValue(Target);
		inner.AdditionalCompilerArguments += "-DPATH_TO_USER_DEFINED_GAME_OPTION=<modern_durak_game_option/gameOption.hxx>";
		inner.bEnableCppCoroutinesForEvaluation=true;
	}


	
	
}
