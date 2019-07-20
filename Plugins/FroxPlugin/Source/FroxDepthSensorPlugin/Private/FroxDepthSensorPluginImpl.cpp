// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FroxDepthSensorPluginImpl.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Shared.h"

#include "Frox/FroxDepthSensor/DepthSensorModule.h"

#define LOCTEXT_NAMESPACE "FroxDepthSensorPlugin"

DEFINE_LOG_CATEGORY(LogFroxDepthSensor);

void FFroxDepthSensorPlugin::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("FroxPlugin")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/FroxDepthSensor.dll"));
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/Mac/Release/libFroxDepthSensor.dylib"));
#endif // PLATFORM_WINDOWS

	LibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	if (LibraryHandle)
	{
		frox::IDepthSensorModule::Get().InitialiseModule();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FFroxDepthSensorPlugin::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (LibraryHandle != nullptr)
	{
		frox::IDepthSensorModule::Get().ShutdownModule();

		// Free the dll handle
		FPlatformProcess::FreeDllHandle(LibraryHandle);
		LibraryHandle = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFroxDepthSensorPlugin, FroxDepthSensorPlugin)
