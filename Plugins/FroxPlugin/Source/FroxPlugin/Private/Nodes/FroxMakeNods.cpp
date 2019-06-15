#include "Nodes/FroxMakeNods.h"
#include "OperationLogic.h"
#include "Utils.h"
#include "FroxPlugin.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/MakeFrameComputeNode.h"

#define LOCTEXT_NAMESPACE "UMakeFrameNode"

frox::ComputeNode* UMakeFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto MakeNode = Flow->CreateNode<frox::MakeFrameComputeNode>();
	check(MakeNode != nullptr);

	MakeNode->SetWidth(Width);
	MakeNode->SetHeight(Width);

	if (Type == EFroxTypeEnum::FTE_None)
	{
		UE_LOG(LogFrox, Error, TEXT("Type isn't set!"));
		return MakeNode;
	}

	frox::EComputeFrameType type = UFroxComputeFrame::UETypeToFroxType(Type);
	check(type != frox::EComputeFrameType::ECFT_None);

	MakeNode->SetType(type);

	switch (type)
	{
	case frox::ECFT_Bool:
		MakeNode->SetValue(Value.ToBool());
		break;
	case frox::ECFT_UInt8:
		MakeNode->SetValue(FCString::Atoi(*Value));
		break;
	case frox::ECFT_UInt16:
		MakeNode->SetValue(FCString::Atoi(*Value));
		break;
	case frox::ECFT_UInt32:
		MakeNode->SetValue(FCString::Atoi(*Value));
		break;
	case frox::ECFT_Float:
		MakeNode->SetValue(FCString::Atof(*Value));
		break;
	default:
		break;
	}

	return MakeNode;
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

FLinearColor UMakeZeroFrameNode::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 0.f);
}
#endif //WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE