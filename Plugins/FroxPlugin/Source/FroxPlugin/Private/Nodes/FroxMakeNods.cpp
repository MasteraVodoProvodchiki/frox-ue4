#include "Nodes/FroxMakeNods.h"
#include "OperationLogic.h"
#include "Utils.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/MakeFrameComputeNode.h"

frox::ComputeNode* UMakeFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	auto makeNode = Flow->CreateNode<frox::MakeFrameComputeNode>();
	makeNode->SetWidth(Width);
	makeNode->SetHeight(Width);

	frox::EComputeFrameType type = KeyTypeToFroxType(Type);
	check(type != frox::EComputeFrameType::ECFT_None);

	makeNode->SetType(type);

	switch (type)
	{
	case frox::ECFT_Bool:
		makeNode->SetValue(Value.ToBool());
		break;
	case frox::ECFT_UInt8:
		makeNode->SetValue(FCString::Atoi(*Value));
		break;
	case frox::ECFT_UInt16:
		makeNode->SetValue(FCString::Atoi(*Value));
		break;
	case frox::ECFT_UInt32:
		makeNode->SetValue(FCString::Atoi(*Value));
		break;
	case frox::ECFT_Float:
		makeNode->SetValue(FCString::Atof(*Value));
		break;
	default:
		break;
	}

	return makeNode;
}

#if WITH_EDITORONLY_DATA
void UMakeFrameNode::AllocateDefaultPins()
{
/*
	UEdGraphPin* Width = CreatePin(EGPD_Input, TEXT(""), TEXT("Width"));
	Width->DefaultValue = "1";
	UEdGraphPin* Height = CreatePin(EGPD_Input, TEXT(""), TEXT("Height"));
	Height->DefaultValue = "1";
*/
	UEdGraphPin* Output = CreatePin(EGPD_Output, TEXT(""), TEXT("Out"));
}

FText UMakeFrameNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTypeName());
}
#endif //WITH_EDITORONLY_DATA

frox::ComputeNode* UMakeZeroFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	return Flow->CreateNode(this->GetTypeName());
	// set w/h
}

#if WITH_EDITORONLY_DATA
FText UMakeZeroFrameNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTypeName());
}
#endif //WITH_EDITORONLY_DATA