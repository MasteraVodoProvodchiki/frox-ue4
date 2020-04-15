// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class IFroxKinect2Plugin : public IModuleInterface
{
public:
	static inline IFroxKinect2Plugin& Get()
	{
		return FModuleManager::LoadModuleChecked<IFroxKinect2Plugin>("FroxKinect2Plugin");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FroxKinect2Plugin");
	}
};
