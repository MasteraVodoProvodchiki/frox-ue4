#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraph.h"
#include "FroxComputeFlowGraph.generated.h"

UCLASS()
class UFroxComputeFlowGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()

public:
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
private:
	UEdGraphNode* CreateInputNode();
	UEdGraphNode* CreateOutputNode();
};
