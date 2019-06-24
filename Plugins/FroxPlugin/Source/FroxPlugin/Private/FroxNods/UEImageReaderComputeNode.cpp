/*
#include "FroxNods/UEImageReaderComputeNode.h"
#include "Frox/Frox/ComputeTask.h"

namespace frox {

/// UEImageReaderComputeNode
UEImageReaderComputeNode::UEImageReaderComputeNode(const ComputeNodeInitializer& initializer)
	: Super(initializer)
{}

UEImageReaderComputeNode::~UEImageReaderComputeNode()
{}

void UEImageReaderComputeNode::AllocateDefaultPins()
{
	_output = CreateOutput("output");
}

void UEImageReaderComputeNode::OnPostInit()
{
	if (_frame)
	{
		SetOutput(_output, _frame);
	}
}

bool UEImageReaderComputeNode::IsValid() const
{
	return
		GetOutput(_output) != nullptr;
}

ComputeTask* UEImageReaderComputeNode::CreateComputeTask()
{
	ComputeFramePtr output = GetOutput(_output);
	return ComputeTaskUtils::Make([output]() {
		// Nothing
	});
}

void UEImageReaderComputeNode::SetFrame(ComputeFramePtr frame)
{
	_frame = frame;
}

FROX_COMPUTENODE_IMPL(UEImageReaderComputeNode)

} // End frox
*/