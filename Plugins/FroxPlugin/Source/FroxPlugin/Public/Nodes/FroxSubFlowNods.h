#pragma once

#include "FroxNods.h"
#include "FroxSubFlowNods.generated.h"

class UFroxComputeFlowAsset;

UCLASS(DisplayName = "SubFlow Operation Node")
class FROXPLUGIN_API USubFlowNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "subflow"; }
	virtual const char* GetTitle() const override { return "SubFlow"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ Begin UOpartionNode

#if WITH_EDITORONLY_DATA
	//~ Begin UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ Begin UObject

	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

public:
	/** value of frame  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UFroxComputeFlowAsset* ComputeFlow;
};