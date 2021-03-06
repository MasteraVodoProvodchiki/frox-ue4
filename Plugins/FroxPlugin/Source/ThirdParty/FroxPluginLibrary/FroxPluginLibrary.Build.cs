// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;
using UnrealBuildTool;

public class FroxPluginLibrary : ModuleRules
{
    private void CopyFile(string FilePath, string DirectoryTo)
    {
        string FileName = Path.GetFileName(FilePath);

        if (!Directory.Exists(DirectoryTo))
            Directory.CreateDirectory(DirectoryTo);

        if (!File.Exists(Path.Combine(DirectoryTo, FileName)))
            File.Copy(FilePath, Path.Combine(DirectoryTo, FileName), true);
    }

    public FroxPluginLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../Frox/", "Frox"));
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../FroxDepthSensor/", "Frox"));

            // Add the import library
            string BinariesPath = Path.Combine(ModuleDirectory, "../Frox/", "x64", "Release");
            PublicLibraryPaths.Add(BinariesPath);
            PublicAdditionalLibraries.Add("Frox.lib");
            PublicAdditionalLibraries.Add("FroxDepthSensor.lib");
            PublicAdditionalLibraries.Add("FroxIntelRealSense.lib");
            PublicAdditionalLibraries.Add("FroxAstra.lib");

            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("Frox.dll");
            PublicDelayLoadDLLs.Add("FroxDepthSensor.dll");
            PublicDelayLoadDLLs.Add("FroxIntelRealSense.dll");
            PublicDelayLoadDLLs.Add("FroxAstra.dll");

            string PlatformString = Target.Platform.ToString();
            string ProjectPath = Path.Combine(ModuleDirectory, "../../..");
            string ProjectBinariesPath = Path.Combine(ProjectPath, "Binaries/", PlatformString);
            CopyFile(Path.Combine(BinariesPath, "Frox.dll"), ProjectBinariesPath);
            CopyFile(Path.Combine(BinariesPath, "FroxDepthSensor.dll"), ProjectBinariesPath);
            CopyFile(Path.Combine(BinariesPath, "FroxIntelRealSense.dll"), ProjectBinariesPath);
            CopyFile(Path.Combine(BinariesPath, "FroxAstra.dll"), ProjectBinariesPath);
            CopyFile(Path.Combine(BinariesPath, "realsense2.dll"), ProjectBinariesPath);
            CopyFile(Path.Combine(BinariesPath, "opencv_world410.dll"), ProjectBinariesPath);

            CopyFile(Path.Combine(BinariesPath, "astra.dll"), ProjectBinariesPath);
            CopyFile(Path.Combine(BinariesPath, "astra_core.dll"), ProjectBinariesPath);
            CopyFile(Path.Combine(BinariesPath, "astra_core_api.dll"), ProjectBinariesPath);
            CopyFile(Path.Combine(BinariesPath, "ORBBEC.dll"), ProjectBinariesPath);
            CopyFile(Path.Combine(BinariesPath, "orbbec.ini"), ProjectBinariesPath);
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libFrox.dylib"));
        }
    }
}
