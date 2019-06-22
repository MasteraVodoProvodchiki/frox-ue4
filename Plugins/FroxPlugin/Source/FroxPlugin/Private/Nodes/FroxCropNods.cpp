#include "FroxCropNods.h"

#include "FroxPlugin.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/CropComputeNode.h"

#define LOCTEXT_NAMESPACE "UCropNods"

/// Make By Noise
frox::ComputeNode* UCropOpartionNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto MakeNode = Flow->CreateNode<frox::CropComputeNode>();
	check(MakeNode != nullptr);

	// MakeNode->

	return MakeNode;
}

#undef LOCTEXT_NAMESPACE