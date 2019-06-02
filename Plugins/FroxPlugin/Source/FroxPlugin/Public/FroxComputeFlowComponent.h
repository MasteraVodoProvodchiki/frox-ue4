#pragma once

#include "Components/ActorComponent.h"

#include "FroxComputeFlowComponent.generated.h"

class UFroxComputeFlowAsset;

UCLASS(ClassGroup = (Frox), meta = (BlueprintSpawnableComponent))
class FROXPLUGIN_API UFroxComputeFlowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spine)
	UFroxComputeFlowAsset* ComputeFlow;

	void GenerateFlow();
};