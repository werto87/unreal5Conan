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
		//CppStandard = CppStandardVersion.Cpp20; //another possible options are "Cpp17", "Latest", not sure about 20 
		PublicDependencyModuleNames.AddRange(new string[] {"MyBoilerplateModule", "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
        Type type = Target.GetType();
        FieldInfo innerField = type.GetField("Inner", BindingFlags.Instance | BindingFlags.NonPublic);
        TargetRules inner = (TargetRules)innerField.GetValue(Target);
        inner.AdditionalCompilerArguments += " -std=c++20 -D _LIBCPP_ENABLE_CXX20_REMOVED_TYPE_TRAITS -fcoroutines-ts -Wno-unused-result";
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	
	}


	
	
}
