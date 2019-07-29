#pragma once

#include "FroxNods.h"
#include "FroxTaskNode.generated.h"

class UFroxTask_BlueprintBase;

UCLASS()
class FROXPLUGIN_API UFroxTaskNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "task"; }
	virtual const char* GetTitle() const override { return "Taks"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ Begin UOpartionNode

	//~ Begin UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ Begin UObject

#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

public:
	/** value of frame  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UFroxTask_BlueprintBase* Task;

	void OnReceiveExecute();
};