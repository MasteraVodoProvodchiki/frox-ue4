#pragma once

#include "FroxComputeFrame.h"
#include "FroxBlueprintLibrary.generated.h"

UCLASS()
class UFroxBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Frox")
	static UFroxComputeFrame* MakeFrame(int Width = 1, int Height = 1, EFroxTypeEnum Type = EFroxTypeEnum::FTE_Float);

};