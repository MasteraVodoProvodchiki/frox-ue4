#pragma once

#include "FroxArithmeticNods.h"
#include "FroxComputeFrame.h"
#include "FroxInRangeNode.generated.h"

UCLASS(DisplayName = "InRange Operation Node")
class FROXPLUGIN_API UFroxInRangeNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "inrange"; }
	virtual const char* GetTitle() const override { return "InRange"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Low = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float High = 1.f;
};