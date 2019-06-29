#include "Nodes/FroxConstNods.h"
#include "Utils.h"
#include "FroxPlugin.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/MakeFrameComputeNode.h"

#define LOCTEXT_NAMESPACE "ConstFrameNode"

/// Base
#if WITH_EDITORONLY_DATA
void UConstFrameNode::AllocateDefaultPins()
{
	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Frame, TEXT("Out"));
}

FText UConstFrameNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}

FLinearColor UConstFrameNode::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 0.f);
}
#endif //WITH_EDITORONLY_DATA

/// Make By Value
frox::ComputeNode* UConstFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto Node = Flow->CreateNode<frox::ConstFrameComputeNode>();
	check(Node != nullptr);

	if (ComputeFrame != nullptr)
	{
		Node->SetFrame(ComputeFrame->GetFroxFrame());
	}

	return Node;
}


#undef LOCTEXT_NAMESPACE