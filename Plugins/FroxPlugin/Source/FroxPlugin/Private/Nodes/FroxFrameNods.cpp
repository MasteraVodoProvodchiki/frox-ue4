#include "Nodes/FroxFrameNods.h"

#include "FroxPlugin.h"
#include "FroxTypes.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/FrameInfoComputeNode.h"

#define LOCTEXT_NAMESPACE "FroxFrameNods"

/// ConvertTo
#if WITH_EDITORONLY_DATA
void UFrameSizeOpartionNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CreatePin(EGPD_Input, UFroxNodeBase::PC_Frame, TEXT("In"));
	CreatePin(EGPD_Output, UFroxNodeBase::PC_Property, FFroxTypeDefinition::IntType, TEXT("Size"));
}

FLinearColor UFrameSizeOpartionNode::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 1.f);
}
#endif

frox::ComputeNode* UFrameSizeOpartionNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto MakeNode = Flow->CreateNode<frox::FrameSizeComputeNode>();
	check(MakeNode != nullptr);

	return MakeNode;
}

#undef LOCTEXT_NAMESPACE