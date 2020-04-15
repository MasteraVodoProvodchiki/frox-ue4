#include "FroxMapFrameNods.h"
#include "Shared.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/FroxDepthSensor/MapFrameNode.h"

#define LOCTEXT_NAMESPACE "UFroxMapDepthToColorNode"

#if WITH_EDITORONLY_DATA
void UFroxMapDepthToColorNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UFroxNodeBase::PC_Frame, TEXT("DepthFrame"));
	CreatePin(EGPD_Input, UFroxNodeBase::PC_Frame, TEXT("ColorFrame"));

	if (ENodeAdvancedPins::NoPins == AdvancedPinDisplay)
	{
		AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	}

	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Frame, TEXT("Out"));
}
#endif //WITH_EDITORONLY_DATA

frox::ComputeNode* UFroxMapDepthToColorNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	frox::MapDepthToColorNode* Node = Flow->CreateNode<frox::MapDepthToColorNode>();
	check(Node != nullptr);
	return Node;
}

#undef LOCTEXT_NAMESPACE
