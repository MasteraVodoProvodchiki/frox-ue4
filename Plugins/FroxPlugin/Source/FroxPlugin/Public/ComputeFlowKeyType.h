#pragma once

#pragma once

#include "ComputeFlowKeyType.generated.h"

UCLASS(Blueprintable, HideDropDown)
class FROXPLUGIN_API UComputeFlowKeyType : public UObject
{
	GENERATED_BODY()

public:

};

UCLASS(Blueprintable)
class UComputeFlowKeyType_Uint8 : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};

UCLASS(Blueprintable)
class UComputeFlowKeyType_Float : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};

UCLASS(Blueprintable)
class UComputeFlowKeyType_Bool : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};

UCLASS(Blueprintable)
class UComputeFlowKeyType_Frame : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};