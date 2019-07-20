#include "FroxSensorFrameNods.h"
#include "Shared.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/FroxDepthSensor/SensorFrameNode.h"
#define LOCTEXT_NAMESPACE "UMakeFrameNode"

/// Base
#if WITH_EDITORONLY_DATA
void UFroxSensorFrameNode::AllocateDefaultPins()
{
	///

	if (ENodeAdvancedPins::NoPins == AdvancedPinDisplay)
	{
		AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	}

	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Frame, TEXT("Out"));
}

FText UFroxSensorFrameNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}

FLinearColor UFroxSensorFrameNode::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 0.f);
}
#endif //WITH_EDITORONLY_DATA

frox::ComputeNode* UFroxSensorDepthFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	frox::SensorDepthFrameNode* SensorFrameNode = Flow->CreateNode<frox::SensorDepthFrameNode>();
	return SensorFrameNode;
	// set w/h
}

frox::ComputeNode* UFroxSensorColorFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	frox::SensorColorFrameNode* SensorFrameNode = Flow->CreateNode<frox::SensorColorFrameNode>();
	return SensorFrameNode;
	// set w/h
}

frox::ComputeNode* UFroxSensorInfraredFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	frox::SensorInfraredFrameNode* SensorFrameNode = Flow->CreateNode<frox::SensorInfraredFrameNode>();
	return SensorFrameNode;
	// set w/h
}

#undef LOCTEXT_NAMESPACE