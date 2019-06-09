#include "FroxComputeFlowGraph.h"
#include "EdGraphSchema_FroxEditor.h"

UFroxComputeFlowGraph::UFroxComputeFlowGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Schema = UEdGraphSchema_FroxEditor::StaticClass();
}