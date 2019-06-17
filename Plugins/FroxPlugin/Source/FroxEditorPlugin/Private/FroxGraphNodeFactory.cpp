#include "FroxGraphNodeFactory.h"
#include "SFroxGraphNode.h"
#include "Nodes/FroxNods.h"

FFroxGraphNodeFactory::FFroxGraphNodeFactory()
{
}

TSharedPtr<class SGraphNode> FFroxGraphNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	if (UFroxNodeBase* FroxNode = Cast<UFroxNodeBase>(Node))
	{
		TSharedPtr<SFroxGraphNode> SNode = SNew(SFroxGraphNode, FroxNode);
		return SNode;
	}
	
	return nullptr;
}
