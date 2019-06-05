#include "FroxComputeFlowComponent.h"
#include "FroxComputeFlowAsset.h"
#include "FroxPlugin.h"
#include "FroxNods.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/ComputeFrame.h"
#include "Frox/Frox/ComputeNode.h"

#include "EdGraph/EdGraph.h"

#define LOCTEXT_NAMESPACE "FFroxPluginModule"

void UFroxComputeFlowComponent::BeginPlay()
{
	Super::BeginPlay();

	GenerateFlow();
}

void UFroxComputeFlowComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ReleaseFlow();

	Super::EndPlay(EndPlayReason);
}

void UFroxComputeFlowComponent::GenerateFlow()
{
	// ComputeFlow->
	// Create ComputeFlow by FlowDesc
	frox::Frox* FroxLib = FFroxPluginModule::Get().GetFrox();

	Flow = FroxLib->CreateComputeFlow();

	UEdGraph* UpdateGraph = ComputeFlow->UpdateGraph;

	// Create All Nodes
	struct NodePair
	{
		UOpartionNode* OperationGraphNode;
		frox::ComputeNode* ComputeNode;
	};


	// Group
	TArray<NodePair> Pairs;
	TArray<UInputPropertyNode*> Inputs;
	TArray<UOutputPropertyNode*> Outputs;
	for (UEdGraphNode* Node : UpdateGraph->Nodes)
	{
		UOpartionNode* OperationGraphNode = Cast<UOpartionNode>(Node);
		if (OperationGraphNode)
		{
			frox::ComputeNode* ComputeNode = Flow->CreateNode(OperationGraphNode->GetTypeName());

			Pairs.Add(NodePair{
				OperationGraphNode,
				ComputeNode
			});
			continue;
		}

		UInputPropertyNode* InputGraphNode = Cast<UInputPropertyNode>(Node);
		if (InputGraphNode)
		{
			Inputs.Add(InputGraphNode);
			continue;
		}

		UOutputPropertyNode* OutputGraphNode = Cast<UOutputPropertyNode>(Node);
		if (OutputGraphNode)
		{
			Outputs.Add(OutputGraphNode);
		}
	}

	// Set inputs
	for (UInputPropertyNode* Input : Inputs)
	{
		Input->PropertyName;

		// Only for output
		TArray<UEdGraphPin*> OutputPins = Input->Pins.FilterByPredicate([](UEdGraphPin* Pin) {
			return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
		});
		for (UEdGraphPin* Pin : OutputPins)
		{
			// Every input
			for (UEdGraphPin* LinkedTo : Pin->LinkedTo)
			{
				UOpartionNode* NextNode = Cast<UOpartionNode>(LinkedTo->GetOwningNode());
				if (NextNode)
				{
					// Get In/Out
					NodePair* InPair = Pairs.FindByPredicate([NextNode](NodePair Pair) {
						return Pair.OperationGraphNode == NextNode;
					});

					if (InPair)
					{
						frox::ComputeNode* InComputeNode = InPair->ComputeNode;
		
						int32 InPinIndex = NextNode->Pins
							.FilterByPredicate([](UEdGraphPin* Pin) {
								return Pin->Direction == EEdGraphPinDirection::EGPD_Input;
							})
							.IndexOfByPredicate([LinkedTo](UEdGraphPin* Pin) {
								return Pin == LinkedTo;
							});

						// Create Input By Prop
						frox::ComputeFramePtr frame = FroxLib->CreateScalar(1.f); // Input->PropertyName;
						InComputeNode->SetInput(InPinIndex, frame);
					}
				}
			}
		}
	}

	// Connect operations
	for (const NodePair& OutPair : Pairs)
	{
		UOpartionNode* OperationGraphNode = OutPair.OperationGraphNode;
		frox::ComputeNode* OutComputeNode = OutPair.ComputeNode;

		// Only for output
		TArray<UEdGraphPin*> OutputPins = OperationGraphNode->Pins.FilterByPredicate([](UEdGraphPin* Pin) {
			return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
		});
		for (int32 OutPinIndex = 0; OutPinIndex < OutputPins.Num(); ++OutPinIndex)
		{
			UEdGraphPin* Pin = OutputPins[OutPinIndex];

			// Every input
			for (UEdGraphPin* LinkedTo : Pin->LinkedTo)
			{
				UOpartionNode* NextNode = Cast<UOpartionNode>(LinkedTo->GetOwningNode());
				if (NextNode)
				{
					// Get In/Out
					NodePair* InPair = Pairs.FindByPredicate([NextNode](NodePair Pair) {
						return Pair.OperationGraphNode == NextNode;
					});

					if (InPair)
					{
						frox::ComputeNode* InComputeNode = InPair->ComputeNode;
						int32 InPinIndex = NextNode->Pins
							.FilterByPredicate([](UEdGraphPin* Pin) {
								return Pin->Direction == EEdGraphPinDirection::EGPD_Input;
							})
							.IndexOfByPredicate([LinkedTo](UEdGraphPin* Pin) {
								return Pin == LinkedTo;
							});

						Flow->ConnectNodes(OutComputeNode, OutPinIndex, InComputeNode, InPinIndex);
					}
				}
			} // End Every In Pin
		} // End Every Out Pin
	} // End every node

	// Set outputs
	for (UOutputPropertyNode* Output : Outputs)
	{
		Output->PropertyName;

		// Only for inputs
		TArray<UEdGraphPin*> InputPins = Output->Pins.FilterByPredicate([](UEdGraphPin* Pin) {
			return Pin->Direction == EEdGraphPinDirection::EGPD_Input;
		});
		for (UEdGraphPin* Pin : InputPins)
		{
			// Every input
			for (UEdGraphPin* LinkedTo : Pin->LinkedTo)
			{
				UOpartionNode* NextNode = Cast<UOpartionNode>(LinkedTo->GetOwningNode());
				if (NextNode)
				{
					// Get In/Out
					NodePair* OutPair = Pairs.FindByPredicate([NextNode](NodePair Pair) {
						return Pair.OperationGraphNode == NextNode;
					});

					if (OutPair)
					{
						frox::ComputeNode* InComputeNode = OutPair->ComputeNode;
						int32 OutPinIndex = NextNode->Pins
							.FilterByPredicate([](UEdGraphPin* Pin) {
								return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
							})
							.IndexOfByPredicate([LinkedTo](UEdGraphPin* Pin) {
								return Pin == LinkedTo;
							});

						frox::ComputeFramePtr frame = InComputeNode->GetOutput(OutPinIndex);
						// Input->PropertyName;
					}
				}
			}
		}
	}
}

void UFroxComputeFlowComponent::ReleaseFlow()
{
	if (Flow)
	{
		frox::Frox* FroxLib = FFroxPluginModule::Get().GetFrox();
		FroxLib->DestroyComputeFlow(Flow);
	}
}

#undef LOCTEXT_NAMESPACE