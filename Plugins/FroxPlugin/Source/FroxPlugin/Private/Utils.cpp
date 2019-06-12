#include "Utils.h"

frox::EComputeFrameType KeyTypeToFroxType(UComputeFlowKeyType* KeyType)
{
	UComputeFlowKeyType_Bool* boolType = Cast<UComputeFlowKeyType_Bool>(KeyType);
	if (boolType)
	{
		return frox::EComputeFrameType::ECFT_Bool;
	}

	UComputeFlowKeyType_Uint8* uint8Type = Cast<UComputeFlowKeyType_Uint8>(KeyType);
	if (uint8Type)
	{
		return frox::EComputeFrameType::ECFT_UInt8;
	}

	UComputeFlowKeyType_Uint16* uint16Type = Cast<UComputeFlowKeyType_Uint16>(KeyType);
	if (uint16Type)
	{
		return frox::EComputeFrameType::ECFT_UInt16;
	}

	UComputeFlowKeyType_Uint32* uint32Type = Cast<UComputeFlowKeyType_Uint32>(KeyType);
	if (uint32Type)
	{
		return frox::EComputeFrameType::ECFT_UInt32;
	}

	UComputeFlowKeyType_Float* floatType = Cast<UComputeFlowKeyType_Float>(KeyType);
	if (floatType)
	{
		return frox::EComputeFrameType::ECFT_Float;
	}

	return frox::EComputeFrameType::ECFT_None;
}