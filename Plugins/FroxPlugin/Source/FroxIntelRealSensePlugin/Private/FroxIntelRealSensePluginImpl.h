// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FroxIntelRealSensePlugin.h"

class FFroxIntelRealSensePlugin : public IFroxIntelRealSensePlugin
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the test dll we will load */
	void* LibraryHandle = nullptr;
};
