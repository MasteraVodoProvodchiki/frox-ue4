// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class FroxDepthSensorPlugin : ModuleRules
{
    private void CopyFile(string FilePath, string DirectoryTo)
    {
        string FileName = Path.GetFileName(FilePath);

        if (!Directory.Exists(DirectoryTo))
            Directory.CreateDirectory(DirectoryTo);

        if (!File.Exists(Path.Combine(DirectoryTo, FileName)))
            File.Copy(FilePath, Path.Combine(DirectoryTo, FileName), true);
    }

    public FroxDepthSensorPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
        /*
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../ThirdParty/FroxDepthSensor/", "Frox"));

            // Add the import library
            string BinariesPath = Path.Combine(ModuleDirectory, "../ThirdParty/Frox/", "x64", "Release");
            PublicLibraryPaths.Add(BinariesPath);
            PublicAdditionalLibraries.Add("FroxDepthSensor.lib");

            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("FroxDepthSensor.dll");

            string PlatformString = Target.Platform.ToString();
            string ProjectPath = Path.Combine(ModuleDirectory, "../..");
            string ProjectBinariesPath = Path.Combine(ProjectPath, "Binaries/", PlatformString);
            CopyFile(Path.Combine(BinariesPath, "FroxDepthSensor.dll"), ProjectBinariesPath);
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libFroxDepthSensor.dylib"));
        }
        */

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
