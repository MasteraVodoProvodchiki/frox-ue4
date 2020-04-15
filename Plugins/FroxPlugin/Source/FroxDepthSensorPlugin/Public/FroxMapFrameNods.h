#pragma once

#include "Nodes/FroxNods.h"
#include "FroxMapFrameNods.generated.h"


UCLASS(DisplayName = "MapDepthFrameToColorFrame Operation Node")
class FROXDEPTHSENSORPLUGIN_API UFroxMapDepthToColorNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "mapdepthtocolor"; }
	virtual const char* GetTitle() const override { return "MapDepthFrameToColorFrame"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA
};