// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"

class IFroxDepthSensorPlugin : public IModuleInterface
{
public:
	static inline IFroxDepthSensorPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IFroxDepthSensorPlugin>("FroxDepthSensorPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FroxDepthSensorPlugin");
	}
};
