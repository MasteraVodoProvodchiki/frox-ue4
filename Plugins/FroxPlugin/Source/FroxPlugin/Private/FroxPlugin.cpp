// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FroxPlugin.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Frox/Frox/Frox.h"

#define LOCTEXT_NAMESPACE "FFroxPluginModule"

void FFroxPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("FroxPlugin")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/FroxPluginLibrary/Win64/Frox.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FroxPluginLibrary/Mac/Release/libFrox.dylib"));
#endif // PLATFORM_WINDOWS

	FroxLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (FroxLibraryHandle)
	{
		// Call the test function in the third party library that opens a message box
		froxLib = frox::FroxInit();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FFroxPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (froxLib)
	{
		// Call the test function in the third party library that opens a message box
		frox::FroxShutdown(froxLib);
		froxLib = nullptr;
	}

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(FroxLibraryHandle);
	FroxLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFroxPluginModule, FroxPlugin)
