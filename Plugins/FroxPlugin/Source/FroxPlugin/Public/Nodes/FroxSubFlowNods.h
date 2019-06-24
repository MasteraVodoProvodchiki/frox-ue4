#pragma once

#include "FroxNods.h"
#include "FroxSubFlowNods.generated.h"

class UFroxComputeFlowAsset;

UCLASS()
class FROXPLUGIN_API USubFlowNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "subflow"; }
	virtual const char* GetTitle() const override { return "SubFlow"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ Begin UOpartionNode

	//~ Begin UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ Begin UObject

#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode

	bool ReallocatePins();
#endif //WITH_EDITORONLY_DATA

public:
	/** value of frame  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UFroxComputeFlowAsset* ComputeFlow;
};