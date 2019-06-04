#include "FroxComputeFlowComponent.h"
#include "FroxComputeFlowAsset.h"
#include "FroxPlugin.h"
#include "FroxNods.h"

#include "Frox/Frox/ComputeFlow.h"

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
		UEdGraphNode* GraphNode;
		frox::ComputeNode* ComputeNode;
	};

	TArray<NodePair> Pairs;
	for (UEdGraphNode* Node : UpdateGraph->Nodes)
	{
		UFroxNodeBase* FroxGraphNode = Cast<UFroxNodeBase>(Node);
		if (FroxGraphNode)
		{
			frox::ComputeNode* ComputeNode = Flow->CreateNode(FroxGraphNode->GetTypeName());

			Pairs.Add(NodePair{
				Node,
				ComputeNode
			});
		}	
	}

	for (UEdGraphNode* Node : UpdateGraph->Nodes)
	{
		// Every pin
		for (UEdGraphPin* Pin : Node->Pins)
		{
			// Only for output
			if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
			{
				// Every input
				for (int32 OutPinIndex = 0; OutPinIndex < Pin->LinkedTo.Num(); ++OutPinIndex)
				{
					UEdGraphPin* LinkedTo = Pin->LinkedTo[OutPinIndex];
					UEdGraphNode* NextNode = LinkedTo->GetOwningNode();

					// Get In/Out
					NodePair* OutPair = Pairs.FindByPredicate([Node](NodePair Pair) {
						return Pair.GraphNode == Node;
					});

					NodePair* InPair = Pairs.FindByPredicate([NextNode](NodePair Pair) {
						return Pair.GraphNode == NextNode;
					});

					if (OutPair && InPair)
					{
						frox::ComputeNode* OutComputeNode = OutPair->ComputeNode;
						frox::ComputeNode* InComputeNode = InPair->ComputeNode;
						int32 InPinIndex = NextNode->Pins.IndexOfByPredicate([LinkedTo](UEdGraphPin* Pin) {
							return Pin == LinkedTo;
						});

						Flow->ConnectNodes(OutComputeNode, OutPinIndex, InComputeNode, InPinIndex);
					}
				} // End Every In Pin
			} // Edn output direction
		} // End Every Out Pin
	} // End every node
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