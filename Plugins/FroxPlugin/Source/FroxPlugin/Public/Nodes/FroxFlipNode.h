#pragma once

#include "FroxArithmeticNods.h"
#include "FroxComputeFrame.h"
#include "FroxFlipNode.generated.h"

UCLASS(DisplayName = "Flip Operation Node")
class FROXPLUGIN_API UFroxFlipNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "flip"; }
	virtual const char* GetTitle() const override { return "Flip"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bVertical = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bHorizontal = true;
};