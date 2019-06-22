#include "FroxConvertNods.h"

#include "FroxPlugin.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/ConvertToComputeNode.h"

#define LOCTEXT_NAMESPACE "UCropNods"

/// Make By Noise
frox::ComputeNode* UConvertToOpartionNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto MakeNode = Flow->CreateNode<frox::ConvertToComputeNode>();
	check(MakeNode != nullptr);

	if (Type == EFroxTypeEnum::FTE_None)
	{
		UE_LOG(LogFrox, Error, TEXT("Type isn't set!"));
		return false;
	}

	frox::EComputeFrameType type = UFroxComputeFrame::UETypeToFroxType(Type);
	check(type != frox::EComputeFrameType::ECFT_None);

	MakeNode->SetType(type);
	MakeNode->SetAlpha(Alpha);
	MakeNode->SetBeta(Beta);

	return MakeNode;
}

#undef LOCTEXT_NAMESPACE