#include "FroxCropNods.h"

#include "FroxPlugin.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/CropComputeNode.h"

#define LOCTEXT_NAMESPACE "UCropNods"

/// Make By Noise
frox::ComputeNode* UCropOpartionNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto CropNode = Flow->CreateNode<frox::CropComputeNode>();
	check(CropNode != nullptr);

	CropNode->SetRect(frox::Rect{X, Y, Width, Height});

	return CropNode;
}

#undef LOCTEXT_NAMESPACE