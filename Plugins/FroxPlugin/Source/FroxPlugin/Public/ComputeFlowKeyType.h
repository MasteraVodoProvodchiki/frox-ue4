#pragma once

#include "ComputeFlowKeyType.generated.h"

UENUM(BlueprintType)
enum class EComputeFlowKeyType : uint8
{
	ECFKT_None			UMETA(DisplayName = "None"),
	ECFKT_Uint8			UMETA(DisplayName = "UInt8"),
	ECFKT_Uint16		UMETA(DisplayName = "UInt16"),
	ECFKT_Uint32		UMETA(DisplayName = "UInt32"),
	ECFKT_Point			UMETA(DisplayName = "Point"),
	ECFKT_Size			UMETA(DisplayName = "Size"),
	ECFKT_Float			UMETA(DisplayName = "Float"),
	ECFKT_Vector2D		UMETA(DisplayName = "Vector2D"),
	ECFKT_Bool			UMETA(DisplayName = "Bool"),
	ECFKT_Frame			UMETA(DisplayName = "Frame"),
	ECFKT_Data			UMETA(DisplayName = "Data"),
};