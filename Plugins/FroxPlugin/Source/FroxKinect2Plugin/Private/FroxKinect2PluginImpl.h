// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FroxKinect2Plugin.h"

class FFroxKinect2Plugin : public IFroxKinect2Plugin
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the test dll we will load */
	void* LibraryHandle = nullptr;
};
