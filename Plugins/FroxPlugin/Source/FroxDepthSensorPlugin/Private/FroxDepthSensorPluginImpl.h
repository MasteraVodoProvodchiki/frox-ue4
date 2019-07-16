// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FroxDepthSensorPlugin.h"

class FFroxDepthSensorPlugin : public IFroxDepthSensorPlugin
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the test dll we will load */
	void* FroxDepthSensorLibraryHandle = nullptr;
	void* FroxIntelRealSenseLibraryHandle = nullptr;
};
