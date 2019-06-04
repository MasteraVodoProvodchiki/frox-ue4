// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

namespace frox {
class Frox;
} // End frox.

class FFroxPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FFroxPluginModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FFroxPluginModule>("FroxPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FroxPlugin");
	}

	frox::Frox* GetFrox() const
	{
		return FroxLib;
	}

private:
	/** Handle to the test dll we will load */
	void*	FroxLibraryHandle;
	frox::Frox* FroxLib;
};
