// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class FroxAstraPlugin : ModuleRules
{
    private void CopyFile(string FilePath, string DirectoryTo)
    {
        string FileName = Path.GetFileName(FilePath);

        if (!Directory.Exists(DirectoryTo))
            Directory.CreateDirectory(DirectoryTo);

        if (!File.Exists(Path.Combine(DirectoryTo, FileName)))
            File.Copy(FilePath, Path.Combine(DirectoryTo, FileName), true);
    }

    public FroxAstraPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "FroxPluginLibrary",
                "FroxPlugin",
                "FroxDepthSensorPlugin",
                "Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
