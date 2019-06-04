#pragma once

#include "Components/ActorComponent.h"
#include "Frox/Frox/Frox.h"

#include "FroxComputeFlowComponent.generated.h"

class UFroxComputeFlowAsset;

UCLASS(ClassGroup = (Frox), meta = (BlueprintSpawnableComponent))
class FROXPLUGIN_API UFroxComputeFlowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spine)
	UFroxComputeFlowAsset* ComputeFlow;

	// UActorComponent
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End UActorComponent

private:
	void GenerateFlow();
	void ReleaseFlow();

private:
	frox::ComputeFlow* Flow;
};