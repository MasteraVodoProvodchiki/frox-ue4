#pragma once

#include "FroxNods.h"
#include "FroxSubFlowNods.generated.h"

class UFroxComputeFlowAsset;

UCLASS()
class FROXPLUGIN_API USubFlowNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "subflow"; }
	virtual const char* GetTitle() const override { return "SubFlow"; }

	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;

	bool ReallocatePins();
#endif //WITH_EDITORONLY_DATA

public:
	/** value of frame  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UFroxComputeFlowAsset* ComputeFlow;
};