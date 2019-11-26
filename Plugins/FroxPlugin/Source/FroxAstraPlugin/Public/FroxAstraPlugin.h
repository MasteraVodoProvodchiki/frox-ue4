// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class IFroxAstraPlugin : public IModuleInterface
{
public:
	static inline IFroxAstraPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IFroxAstraPlugin>("FroxAstraPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FroxAstraPlugin");
	}
};
