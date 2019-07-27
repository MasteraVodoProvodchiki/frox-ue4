#include "Nodes/FroxSubFlowNods.h"
#include "Shared.h"
#include "FroxComputeFlowAsset.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/SubFlowComputeNode.h"

#define LOCTEXT_NAMESPACE "UMakeFrameNode"

frox::ComputeNode* USubFlowNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

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
}

void USubFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(USubFlowNode, ComputeFlow))
	{
		ReallocatePins();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

#if WITH_EDITORONLY_DATA
void USubFlowNode::AllocateDefaultPins()
{
	if (ComputeFlow)
	{
		for (const FComputeFlowEntry& entry : ComputeFlow->Keys)
		{
			FString PinName = entry.EntryName.ToString();
			if (entry.Direction == EComputeFlowEntryDirection::ECFED_Input)
			{			
				CreatePin(EGPD_Input, UFroxNodeBase::PC_Frame, *PinName);
			}
			else if (entry.Direction == EComputeFlowEntryDirection::ECFED_Output)
			{
				CreatePin(EGPD_Output, UFroxNodeBase::PC_Frame, *PinName);
			}
		}
	}
}

FText USubFlowNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}

FLinearColor USubFlowNode::GetNodeTitleColor() const
{
	return FLinearColor(0.f, 0.5f, 1.f);
}
#endif //WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE