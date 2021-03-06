// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FroxEditorPlugin : ModuleRules
{
	public FroxEditorPlugin(ReadOnlyTargetRules Target) : base(Target)
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
                "InputCore",
                "Engine",
                "UnrealEd",
                "FroxPluginLibrary",
                "FroxPlugin",
                "FroxDepthSensorPlugin",

                "AssetTools",
                "AssetRegistry",
                //"EditorStyle",
                // "KismetWidgets",
                // "GraphEditor"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Slate",
                "SlateCore",
                "PropertyEditor",
                "UnrealEd",
                "EditorStyle",
                "GraphEditor",
                "Kismet",
                "ImageWrapper",
                "DesktopPlatform"
				// ... add private dependencies that you statically link with here ...	
			}
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
                //"AssetTools",
                //"AssetRegistry"
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
