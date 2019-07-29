#pragma once

#include "FroxArithmeticNods.h"
#include "FroxComputeFrame.h"
#include "FroxFindContoursNode.generated.h"

UCLASS()
class FROXPLUGIN_API UFroxFindContoursNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "findcontours"; }
	virtual const char* GetTitle() const override { return "FindContours"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode
};