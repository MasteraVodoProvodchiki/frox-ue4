#pragma once

#include "FroxNods.h"
#include "FroxComputeFrame.h"
#include "FroxFrameNods.generated.h"

UCLASS()
class FROXPLUGIN_API UFrameSizeOpartionNode : public UOpartionNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "framesize"; }
	virtual const char* GetTitle() const override { return "FrameSize"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode
};