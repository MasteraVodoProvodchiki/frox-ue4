#pragma once

#include "ComputeFlowKeyType.generated.h"

UCLASS(EditInlineNew, Abstract, CollapseCategories, HideDropDown)
class FROXPLUGIN_API UComputeFlowKeyType : public UObject
{
	GENERATED_BODY()

public:

};

UCLASS(EditInlineNew, meta = (DisplayName = "UInt8"))
class UComputeFlowKeyType_Uint8 : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};

UCLASS(EditInlineNew, meta = (DisplayName = "UInt16"))
class UComputeFlowKeyType_Uint16 : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};

UCLASS(EditInlineNew, meta = (DisplayName = "UInt32"))
class UComputeFlowKeyType_Uint32 : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};

UCLASS(EditInlineNew, meta = (DisplayName = "Float"))
class UComputeFlowKeyType_Float : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};

UCLASS(EditInlineNew, meta = (DisplayName = "Bool"))
class UComputeFlowKeyType_Bool : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};

UCLASS(EditInlineNew, meta = (DisplayName = "Frame"))
class UComputeFlowKeyType_Frame : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};

UCLASS(EditInlineNew, meta = (DisplayName = "Data"))
class UComputeFlowKeyType_Data : public UComputeFlowKeyType
{
	GENERATED_BODY()

public:

};