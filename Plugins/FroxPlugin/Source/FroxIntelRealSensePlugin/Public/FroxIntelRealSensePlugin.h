// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class IFroxIntelRealSensePlugin : public IModuleInterface
{
public:
	static inline IFroxIntelRealSensePlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IFroxIntelRealSensePlugin>("FroxIntelRealSensePlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FroxIntelRealSensePlugin");
	}
};
