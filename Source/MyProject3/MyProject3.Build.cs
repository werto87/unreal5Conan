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
		//TODO could not find out how to use c++20 features. Wait until c++20 support is in unreal engine. can not use concepts or auto as type in function
		//TODO CppStandard = CppStandardVersion.Cpp20; is not compiling
		//TODO in case compiler flags need to be set inner.AdditionalCompilerArguments += " ";
		inner.bEnableCppCoroutinesForEvaluation=true;
	}


	
	
}
