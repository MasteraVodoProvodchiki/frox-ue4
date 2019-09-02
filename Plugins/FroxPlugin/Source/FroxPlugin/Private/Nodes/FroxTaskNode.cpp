#include "Nodes/FroxTaskNode.h"
#include "Shared.h"
#include "FroxComputeFlowAsset.h"
#include "Tasks/FroxTask_BlueprintBase.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/CustomTaskComputeNode.h"

#define LOCTEXT_NAMESPACE "UFroxTaskNode"

DECLARE_DELEGATE(FCustomTaskComputeNodeReceiveExecuteCallBack);

class FCustomTaskComputeNodeListerner : public TSharedFromThis<FCustomTaskComputeNodeListerner>
{
public:
	FCustomTaskComputeNodeReceiveExecuteCallBack OnReceiveExecute;

	FCustomTaskComputeNodeListerner(FCustomTaskComputeNodeReceiveExecuteCallBack InOnReceiveExecute)
		: OnReceiveExecute(InOnReceiveExecute)
	{}
};

namespace frox {

class UE4CustomTaskComputeNode : public CustomTaskComputeNode
{
	FROX_COMPUTENODE(UE4CustomTaskComputeNode, "customtask")
	using Super = frox::CustomTaskComputeNode;

public:
	TSharedPtr<FCustomTaskComputeNodeListerner> Listerner;

	UE4CustomTaskComputeNode(const ComputeNodeInitializer& initializer)
		: Super(initializer)
	{}

	virtual ~UE4CustomTaskComputeNode()
	{}

	virtual bool ReceiveExecute(FlowDataImplPtr inputData, FlowDataImplPtr outputData) override
	{
		// Push Main Thread
		auto L = Listerner;
		AsyncTask(ENamedThreads::GameThread, [L]()
		{
			L->OnReceiveExecute.ExecuteIfBound();
		});

		return true;
	}
};

// FROX_COMPUTENODE_IMPL(UE4CustomTaskComputeNode)

} // End frox


frox::ComputeNode* UFroxTaskNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto CustomTaskNode = Flow->CreateNode<frox::UE4CustomTaskComputeNode>();
	
	FCustomTaskComputeNodeReceiveExecuteCallBack CallBack;
	CallBack.BindUObject(this, &UFroxTaskNode::OnReceiveExecute);
	CustomTaskNode->Listerner = TSharedPtr<FCustomTaskComputeNodeListerner>(new FCustomTaskComputeNodeListerner(CallBack));

	return CustomTaskNode;
}

#if WITH_EDITORONLY_DATA
void UFroxTaskNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UFroxTaskNode, Task))
	{
		ReallocatePins();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

void UFroxTaskNode::AllocateDefaultPins()
{
	if (Task)
	{

	}
}

FText UFroxTaskNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}

FLinearColor UFroxTaskNode::GetNodeTitleColor() const
{
	return FLinearColor(0.f, 0.5f, 1.f);
}
#endif //WITH_EDITORONLY_DATA

void UFroxTaskNode::OnReceiveExecute()
{
	if (Task)
	{
		Task->ReceiveExecute(nullptr);
	}
}

#undef LOCTEXT_NAMESPACE