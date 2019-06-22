#include "Nodes/FroxMakeNods.h"
#include "OperationLogic.h"
#include "Utils.h"
#include "FroxPlugin.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/MakeFrameComputeNode.h"

#define LOCTEXT_NAMESPACE "UMakeFrameNode"

/// Base
#if WITH_EDITORONLY_DATA
void UMakeFrameBaseNode::AllocateDefaultPins()
{
	/*
		UEdGraphPin* Width = CreatePin(EGPD_Input, TEXT(""), TEXT("Width"));
		Width->DefaultValue = "1";
		UEdGraphPin* Height = CreatePin(EGPD_Input, TEXT(""), TEXT("Height"));
		Height->DefaultValue = "1";
	*/
	UEdGraphPin* Output = CreatePin(EGPD_Output, TEXT(""), TEXT("Out"));
}

FText UMakeFrameBaseNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}

FLinearColor UMakeFrameBaseNode::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 0.f);
}
#endif //WITH_EDITORONLY_DATA

bool UMakeFrameBaseNode::FillNode(frox::MakeFrameBaseComputeNode& FroxNode) const
{
	FroxNode.SetWidth(Width);
	FroxNode.SetHeight(Width);

	if (Type == EFroxTypeEnum::FTE_None)
	{
		UE_LOG(LogFrox, Error, TEXT("Type isn't set!"));
		return false;
	}

	frox::EComputeFrameType type = UFroxComputeFrame::UETypeToFroxType(Type);
	check(type != frox::EComputeFrameType::ECFT_None);

	FroxNode.SetType(type);
	return true;
}

/// Make By Value
frox::ComputeNode* UMakeFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto MakeNode = Flow->CreateNode<frox::MakeFrameComputeNode>();
	check(MakeNode != nullptr);

	if (!FillNode(*MakeNode))
	{
		return MakeNode;
	}

	frox::EComputeFrameType type = UFroxComputeFrame::UETypeToFroxType(Type);
	check(type != frox::EComputeFrameType::ECFT_None);

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

/// Make By Zero
frox::ComputeNode* UMakeZeroFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	return Flow->CreateNode(this->GetTypeName());
	// set w/h
}

/// Make By Noise
frox::ComputeNode* UMakeNoiseFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto MakeNode = Flow->CreateNode<frox::MakeNoiseFrameComputeNode>();
	check(MakeNode != nullptr);

	if (!FillNode(*MakeNode))
	{
		return MakeNode;
	}

	return MakeNode;
}


#undef LOCTEXT_NAMESPACE