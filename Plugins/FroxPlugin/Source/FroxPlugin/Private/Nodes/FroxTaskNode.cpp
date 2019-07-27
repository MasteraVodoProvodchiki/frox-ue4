#include "Nodes/FroxTaskNode.h"
#include "Shared.h"
#include "FroxComputeFlowAsset.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/SubFlowComputeNode.h"

#define LOCTEXT_NAMESPACE "UFroxTaskNode"

frox::ComputeNode* UFroxTaskNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	/*
	auto SubFlowNode = Flow->CreateNode<frox::SubFlowComputeNode>();
	if (ComputeFlow)
	{
		frox::ComputeFlow* FroxFlow = ComputeFlow->CreateFlow();
		check(FroxFlow != nullptr);

		SubFlowNode->SetSubFlow(FroxFlow);
	}
	else
	{
		UE_LOG(LogFrox, Error, TEXT("ComputeFlow hasn't been set!"));
	}

	return SubFlowNode;
	*/
	return nullptr;
}

void UFroxTaskNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UFroxTaskNode, Task))
	{
		ReallocatePins();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

#if WITH_EDITORONLY_DATA
void UFroxTaskNode::AllocateDefaultPins()
{
	if (Task)
	{

	}
}

FText UFroxTaskNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}

FLinearColor UFroxTaskNode::GetNodeTitleColor() const
{
	return FLinearColor(0.f, 0.5f, 1.f);
}
#endif //WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE