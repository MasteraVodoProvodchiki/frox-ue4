#pragma once

#include "FroxNods.h"
#include "FroxComputeFrame.h"
#include "UE4CustomNode.generated.h"

UCLASS(Abstract)
class FROXPLUGIN_API UUE4CustomNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	//~ Begin UEdGraphNode
#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
#endif //WITH_EDITORONLY_DATA
	//~ End UOpartionNode

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "UE4CustomNode"; }
	virtual const char* GetTitle() const override { return "UE4CustomNode"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode
};