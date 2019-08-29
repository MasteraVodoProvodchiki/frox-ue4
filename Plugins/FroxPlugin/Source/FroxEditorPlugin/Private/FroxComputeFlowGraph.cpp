#include "FroxComputeFlowGraph.h"
#include "EdGraphSchema_FroxEditor.h"
#include "FroxNods.h"

UFroxComputeFlowGraph::UFroxComputeFlowGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Schema = UEdGraphSchema_FroxEditor::StaticClass();
}

void UFroxComputeFlowGraph::PostInitProperties()
{
	if ((GetFlags() & RF_NeedLoad) != RF_NeedLoad)
	{
		// Create new nodes, but only if graph is not going to be loaded.
		UEdGraphNode* InputNode = CreateInputNode();
		UEdGraphNode* OutputNode = CreateOutputNode();
	}

	// if (!HasAnyFlags(RF_ClassDefaultObject)) importData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	Super::PostInitProperties();
}

void UFroxComputeFlowGraph::PostLoad()
{
	UEdGraphNode* InputNode = nullptr;
	UEdGraphNode* OutputNode = nullptr;
	TArray<UEdGraphNode*> Remove;
	for (UEdGraphNode* Node : Nodes)
	{
		// Input
		if (Node->IsA(UInputPropertyNode::StaticClass()))
		{
			if (InputNode)
			{
				Remove.Add(Node);
			}
			else
			{
				InputNode = Node;
			}
		}
		else if (Node->IsA(UOutputPropertyNode::StaticClass()))
		{
			if (OutputNode)
			{
				Remove.Add(Node);
			}
			else
			{
				OutputNode = Node;
			}
		}
	}

	for (UEdGraphNode* Node : Remove)
	{
		RemoveNode(Node);
	}

	if (!InputNode)
	{
		InputNode = CreateInputNode();
	}
	if (!OutputNode)
	{
		OutputNode = CreateOutputNode();
	}

	NotifyGraphChanged();

	Super::PostLoad();
}

UEdGraphNode* UFroxComputeFlowGraph::CreateInputNode()
{
	UEdGraphNode* InputNode = CreateNode(UInputPropertyNode::StaticClass(), false, false);
	FVector2D Pos = GetGoodPlaceForNewNode();
	InputNode->NodePosX = Pos.X;
	InputNode->NodePosY = Pos.Y;
	return InputNode;
}

UEdGraphNode* UFroxComputeFlowGraph::CreateOutputNode()
{
	UEdGraphNode* OutputNode = CreateNode(UOutputPropertyNode::StaticClass(), false, false);
	FVector2D Pos = GetGoodPlaceForNewNode();
	OutputNode->NodePosX = Pos.X;
	OutputNode->NodePosY = Pos.Y;
	return OutputNode;
}