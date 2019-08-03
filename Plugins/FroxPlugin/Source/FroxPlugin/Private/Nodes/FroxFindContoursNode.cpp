#include "FroxFindContoursNode.h"
#include "Shared.h"
#include "FroxTypes.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/FindContoursComputeNode.h"
#include "Frox/Frox/CenterOfContourComputeNode.h"

#define LOCTEXT_NAMESPACE "UFroxFindContoursNode"

/// FindContours
#if WITH_EDITORONLY_DATA
void UFroxFindContoursNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	UEdGraphPin* In = CreatePin(EGPD_Input, UFroxNodeBase::PC_Frame, TEXT("In"));
	UEdGraphPin* Contours = CreatePin(EGPD_Output, UFroxNodeBase::PC_Data, TEXT("Contours"));
	UEdGraphPin* Hierarchy = CreatePin(EGPD_Output, UFroxNodeBase::PC_Data, TEXT("Hierarchy"));
}
#endif

frox::ComputeNode* UFroxFindContoursNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto FindContoursNode = Flow->CreateNode<frox::FindContoursComputeNode>();
	check(FindContoursNode != nullptr);

	return FindContoursNode;
}

/// CenterOfContour
#if WITH_EDITORONLY_DATA
void UFroxCenterOfContourNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	UEdGraphPin* In = CreatePin(EGPD_Input, UFroxNodeBase::PC_Data, TEXT("In"));
	UEdGraphPin* Out = CreatePin(EGPD_Output, UFroxNodeBase::PC_Data, TEXT("Out"));
}
#endif

frox::ComputeNode* UFroxCenterOfContourNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto CenterOfContourNode = Flow->CreateNode<frox::CenterOfContourComputeNode>();
	check(CenterOfContourNode != nullptr);

	return CenterOfContourNode;
}

#undef LOCTEXT_NAMESPACE