#include "Nodes/UE4CustomNode.h"
#include "Utils.h"
#include "Shared.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/Frox.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/CustomTaskComputeNode.h"
#include "Frox/Frox/ComputeTask.h"
#include "Frox/Frox/ComputeTaskHelper.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "UUE4CustomNode"

namespace frox {

class UE4CustomComputeNode : public ComputeNodeImpl
{
	FROX_COMPUTENODE(UE4CustomComputeNode, "customtask")
	using Super = ComputeNodeImpl;

public:
	UE4CustomComputeNode(const ComputeNodeInitializer& initializer)
		: Super(initializer)
		, _input("input")
		, _output("output")
	{}

	virtual ~UE4CustomComputeNode()
	{}

	virtual void AllocateDefaultPins() override
	{
		RegisterInput(&_input);
		RegisterOutput(&_output);
	}

	virtual ComputeTask* CreateComputeTask(FlowDataImplPtr inputData, FlowDataImplPtr outputData) override
	{
		// Preapre inputs/output
		auto input = _input.GetValue(inputData);
		auto output = _output.GetValue(outputData);

		// Make task
		return
			ComputeTaskHelper::UnPackProps(input)
			// .Validate
			// .UnPackOutputs
			// .Invoke
			.MakeTask(
				[](ComputeFramePtr input) {
					return input != nullptr && input->IsValid();
				},
				[output](ComputeFramePtr input) {
					Size inputSize = input->GetSize();
					Size outputSize = inputSize;

					output.SetValue(
						outputSize,
						input->GetType(),
						[input](ComputeFramePtr output) {
							
							frox::Size size = input->GetSize();
							uint32_t elementSize = input->GetElementSize();

							for (int32 y = 0; y < int32(size.Height); ++y)
							for (int32 x = 0; x < int32(size.Width); ++x)
							{
								int32 ix = int32(size.Width) - x - 1;
								memcpy(output->At(frox::Point{ ix, y }), input->At(frox::Point{ x, y }), elementSize);
							}
						}
					);	
				}
			);
	}

private:
	TExpressionInput<ComputeFramePtr> _input;
	TOutputFrame<ComputeFramePtr> _output;
};

// FROX_COMPUTENODE_IMPL(UE4CustomComputeNode)

} // End frox

/// Base
#if WITH_EDITORONLY_DATA
void UUE4CustomNode::AllocateDefaultPins()
{
	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Frame, TEXT("Out"));
}

FText UUE4CustomNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}

FLinearColor UUE4CustomNode::GetNodeTitleColor() const
{
	return FLinearColor(1.f, 0.5f, 0.f);
}
#endif //WITH_EDITORONLY_DATA

/// Make By Value
frox::ComputeNode* UUE4CustomNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto CustomeNode = Flow->CreateNode<frox::UE4CustomComputeNode>();
	check(CustomeNode != nullptr);

	return CustomeNode;
}


#undef LOCTEXT_NAMESPACE