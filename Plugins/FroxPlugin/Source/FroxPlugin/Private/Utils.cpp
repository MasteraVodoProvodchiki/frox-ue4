#include "Utils.h"

frox::EComputeFrameType KeyTypeToFroxType(EComputeFlowKeyType KeyType)
{
	switch (KeyType)
	{
	case EComputeFlowKeyType::ECFKT_Uint8:
		return frox::EComputeFrameType::ECFT_UInt8;
	case EComputeFlowKeyType::ECFKT_Uint16:
		return frox::EComputeFrameType::ECFT_UInt16;
	case EComputeFlowKeyType::ECFKT_Uint32:
		return frox::EComputeFrameType::ECFT_UInt32;
	case EComputeFlowKeyType::ECFKT_Float:
		return frox::EComputeFrameType::ECFT_Float;
	case EComputeFlowKeyType::ECFKT_Bool:
		return frox::EComputeFrameType::ECFT_Bool;
	}

	return frox::EComputeFrameType::ECFT_None;
}