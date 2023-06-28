// Copyright Epic Games, Inc. All Rights Reserved.

using System.Reflection;
using UnrealBuildTool;

public class MyProject3 : ModuleRules
{
	public MyProject3( ReadOnlyTargetRules Target ) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnableExceptions = true;
		PublicDependencyModuleNames.AddRange(new[]
			{ "MyBoilerplateModule", "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] { });
		var type = Target.GetType();
		var innerField = type.GetField("Inner", BindingFlags.Instance | BindingFlags.NonPublic);
		var inner = (TargetRules)innerField.GetValue(Target);
		inner.AdditionalCompilerArguments +=
			"-D PATH_TO_USER_DEFINED_GAME_OPTION=<modern_durak_game_option/gameOption.hxx> -D RUN_AGAINST_REMOTE";
		inner.bEnableCppCoroutinesForEvaluation = true;
	}
}
