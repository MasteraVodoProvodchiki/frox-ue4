#include "SFroxGraphNode.h"

#define LOCTEXT_NAMESPACE "SFroxGraphNode"

SFroxGraphNode::SFroxGraphNode() : SGraphNode()
{
	FroxNode = nullptr;
}

SFroxGraphNode::~SFroxGraphNode()
{
	if (FroxNode.IsValid())
	{
		FroxNode->OnVisualsChanged().RemoveAll(this);
	}
}

void SFroxGraphNode::Construct(const FArguments& InArgs, UEdGraphNode* InGraphNode)
{
	GraphNode = InGraphNode;
	RegisterFroxGraphNode(InGraphNode);
	UpdateGraphNode();
}


void SFroxGraphNode::HandlFroxGraphNodeChanged(UFroxNodeBase* InNode)
{
	check(InNode == FroxNode);
	UpdateGraphNode();
}

void SFroxGraphNode::RegisterFroxGraphNode(UEdGraphNode* InNode)
{
	FroxNode = Cast<UFroxNodeBase>(InNode);
	FroxNode->OnVisualsChanged().AddSP(this, &SFroxGraphNode::HandlFroxGraphNodeChanged);
}

void SFroxGraphNode::UpdateGraphNode()
{
	check(FroxNode.IsValid());
	SGraphNode::UpdateGraphNode();
	LastSyncedNodeChangeId = FroxNode->GetChangeId();
}

#undef LOCTEXT_NAMESPACE