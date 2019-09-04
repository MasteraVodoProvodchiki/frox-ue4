// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

namespace frox {
class Frox;
} // End frox.

class IFroxPlugin : public IModuleInterface
{
public:
	static inline IFroxPlugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IFroxPlugin>("FroxPlugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FroxPlugin");
	}

	virtual frox::Frox* GetFrox() const = 0;
};
