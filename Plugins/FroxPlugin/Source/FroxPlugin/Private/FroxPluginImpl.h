// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "FroxPlugin.h"

class FFroxPlugin : public IFroxPlugin
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** IFroxPlugin implementation */
	virtual frox::Frox* GetFrox() const override
	{
		return FroxLib;
	}

private:
	/** Handle to the test dll we will load */
	void* FroxLibraryHandle = nullptr;
	frox::Frox* FroxLib;
};
