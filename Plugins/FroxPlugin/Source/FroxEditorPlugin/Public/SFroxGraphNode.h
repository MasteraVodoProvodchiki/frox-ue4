#pragma once

#include "SGraphNode.h"
#include "Nodes/FroxNods.h"


class SFroxGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SFroxGraphNode) { }
	SLATE_END_ARGS()

	SFroxGraphNode();
	virtual ~SFroxGraphNode();

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);

	//~ SGraphNode api
	virtual void UpdateGraphNode() override;

protected:
	void RegisterFroxGraphNode(UEdGraphNode* InNode);
	void HandlFroxGraphNodeChanged(UFroxNodeBase* InNode);
	TWeakObjectPtr<UFroxNodeBase> FroxNode;
	FGuid LastSyncedNodeChangeId;
};
