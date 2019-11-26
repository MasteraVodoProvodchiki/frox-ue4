// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FroxAstraPlugin.h"

class FFroxAstraPlugin : public IFroxAstraPlugin
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the test dll we will load */
	void* LibraryHandle = nullptr;
};
