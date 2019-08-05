#pragma once

#include "CoreMinimal.h"
#include "FroxTypes.generated.h"

USTRUCT()
struct FROXPLUGIN_API FFroxTypeDefinition
{
	GENERATED_USTRUCT_BODY()

public:
	static const FName BoolType;
	static const FName IntType;
	static const FName UInt8Type;
	static const FName UInt16Type;
	static const FName UInt32Type;
	static const FName EnumType;
	static const FName FloatType;
	static const FName FrameType;
	static const FName DataType;
};