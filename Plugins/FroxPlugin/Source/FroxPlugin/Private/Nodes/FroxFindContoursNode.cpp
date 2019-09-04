#include "Nodes/FroxFindContoursNode.h"
#include "Shared.h"
#include "FroxTypes.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/FindContoursComputeNode.h"
#include "Frox/Frox/CenterOfContourComputeNode.h"
#include "Frox/Frox/RadiusOfContourComputeNode.h"

#define LOCTEXT_NAMESPACE "UFroxFindContoursNode"

frox::EFindContoursMode FindContoursModeToFroxMode(EFroxFindContoursMode mode)
{
	switch (mode)
	{
	case EFroxFindContoursMode::External:
		return frox::EFindContoursMode::External;
	case EFroxFindContoursMode::List:
		return frox::EFindContoursMode::List;
	case EFroxFindContoursMode::CComp:
		return frox::EFindContoursMode::CComp;
	case EFroxFindContoursMode::Tree:
		return frox::EFindContoursMode::Tree;
	case EFroxFindContoursMode::FloodFill:
		return frox::EFindContoursMode::FloodFill;
	default:
		assert(false);
	}

	return frox::EFindContoursMode::Tree;
}

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

	FindContoursNode->SetMode(FindContoursModeToFroxMode(Mode));
	
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

/// RadiusOfContour
#if WITH_EDITORONLY_DATA
void UFroxRadiusOfContourNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	UEdGraphPin* In = CreatePin(EGPD_Input, UFroxNodeBase::PC_Data, TEXT("In"));
	UEdGraphPin* Out = CreatePin(EGPD_Output, UFroxNodeBase::PC_Data, TEXT("Out"));
}
#endif

frox::ComputeNode* UFroxRadiusOfContourNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto RadiusOfContourNode = Flow->CreateNode<frox::RadiusOfContourComputeNode>();
	check(RadiusOfContourNode != nullptr);

	return RadiusOfContourNode;
}

#undef LOCTEXT_NAMESPACE