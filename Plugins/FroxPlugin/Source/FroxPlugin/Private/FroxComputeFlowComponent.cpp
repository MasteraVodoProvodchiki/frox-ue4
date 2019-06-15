#include "FroxComputeFlowComponent.h"
#include "FroxComputeFlowAsset.h"
#include "FroxComputeFrame.h"
#include "FroxPlugin.h"
#include "Nodes/FroxNods.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/ComputeFrame.h"
#include "Frox/Frox/ComputeNode.h"

#include "EdGraph/EdGraph.h"

#define LOCTEXT_NAMESPACE "FFroxPluginModule"

UFroxComputeFlowComponent::UFroxComputeFlowComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UFroxComputeFlowComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (ComputeFlowAsset)
	{
		InitializeFlow(*ComputeFlowAsset);
	}
}

void UFroxComputeFlowComponent::UninitializeComponent()
{
	ReleaseFlow();
	Super::UninitializeComponent();
}

void UFroxComputeFlowComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ComputeFlow)
	{
		// ComputeFlow->Start();
	}
}

void UFroxComputeFlowComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ComputeFlow)
	{
		// ComputeFlow->Stop();
	}

	Super::EndPlay(EndPlayReason);
}

void UFroxComputeFlowComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_bRunning && ComputeFlow)
	{
		TickFlow();
	}
}

void UFroxComputeFlowComponent::Start()
{
	_bRunning = true;
}

void UFroxComputeFlowComponent::Stop()
{
	_bRunning = false;
}

bool UFroxComputeFlowComponent::IsRunning() const
{
	return _bRunning;
}

int32 UFroxComputeFlowComponent::GetValueAsInt(const FName& KeyName) const
{
	const int32* Found = IntValues.Find(KeyName);
	return Found ? *Found : 0;
}

float UFroxComputeFlowComponent::GetValueAsFloat(const FName& KeyName) const
{
	const float* Found = FloatValues.Find(KeyName);
	return Found ? *Found : 0.f;
}

bool UFroxComputeFlowComponent::GetValueAsBool(const FName& KeyName) const
{
	const bool* Found = BoolValues.Find(KeyName);
	return Found ? *Found : false;
}

UFroxComputeFrame* UFroxComputeFlowComponent::GetValueAsFrame(const FName& KeyName) const
{
	auto Found = FrameValues.Find(KeyName);
	return Found ? *Found : nullptr;
}

void UFroxComputeFlowComponent::SetValueAsInt(const FName& KeyName, int32 IntValue)
{
	IntValues.Add(KeyName, IntValue);
	OnValueChanged.Broadcast(KeyName);
}

void UFroxComputeFlowComponent::SetValueAsFloat(const FName& KeyName, float FloatValue)
{
	FloatValues.Add(KeyName, FloatValue);
	OnValueChanged.Broadcast(KeyName);
}

void UFroxComputeFlowComponent::SetValueAsBool(const FName& KeyName, bool BoolValue)
{
	BoolValues.Add(KeyName, BoolValue);
	OnValueChanged.Broadcast(KeyName);
}

void UFroxComputeFlowComponent::SetValueAsFrame(const FName& KeyName, UFroxComputeFrame* ComputeFrame)
{
	FrameValues.Add(KeyName, ComputeFrame);
	OnValueChanged.Broadcast(KeyName);
}

bool UFroxComputeFlowComponent::InitializeFlow(UFroxComputeFlowAsset& NewAsset)
{
	// Create ComputeFlow by FlowDesc
	frox::Frox* FroxLib = FFroxPluginModule::Get().GetFrox();
	check(FroxLib != nullptr);

	ComputeFlow = FroxLib->CreateComputeFlow();
	check(ComputeFlow != nullptr);

	UEdGraph* UpdateGraph = NewAsset.UpdateGraph;
	check(UpdateGraph != nullptr);


	// NewAsset.Keys;
	for (const FComputeFlowEntry& entry : NewAsset.Keys)
	{
		// entry.
	}

	// Group
	TArray<UInputPropertyNode*> Inputs;
	TArray<UOutputPropertyNode*> Outputs;
	TArray<UOpartionNode*> Operations;
	for (UEdGraphNode* Node : UpdateGraph->Nodes)
	{
		// Node Operation
		UOpartionNode* OperationGraphNode = Cast<UOpartionNode>(Node);
		if (OperationGraphNode)
		{
			Operations.Add(OperationGraphNode);
			continue;
		}

		// Input
		UInputPropertyNode* InputGraphNode = Cast<UInputPropertyNode>(Node);
		if (InputGraphNode)
		{
			Inputs.Add(InputGraphNode);
			continue;
		}

		// Output
		UOutputPropertyNode* OutputGraphNode = Cast<UOutputPropertyNode>(Node);
		if (OutputGraphNode)
		{
			Outputs.Add(OutputGraphNode);
			continue;
		}
	}

	// Create Nodes
	TArray<NodePair> Pairs;
	for (UOpartionNode* Node : Operations)
	{
		frox::ComputeNode* ComputeNode = Node->CreateFroxNode(ComputeFlow);
		check(ComputeNode != nullptr);

		Pairs.Add(NodePair{
			Node,
			ComputeNode
		});
	}

	InitializeFlowOperations(Operations, Pairs);
	InitializeFlowInputs(Inputs, Pairs);
	InitializeFlowOutputs(Outputs, Pairs);
	
	return true;
}

void UFroxComputeFlowComponent::InitializeFlowOperations(const TArray<UOpartionNode*>& Operations, const TArray<NodePair>& Pairs)
{
	check(ComputeFlow)

	// Connect operations
	for (const NodePair& OutPair : Pairs)
	{
		UOpartionNode* OperationGraphNode = OutPair.Key;
		frox::ComputeNode* OutComputeNode = OutPair.Value;

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
					const NodePair* InPair = Pairs.FindByPredicate([NextNode](NodePair Pair) {
						return Pair.Key == NextNode;
					});
					check(InPair != nullptr);

					frox::ComputeNode* InComputeNode = InPair->Value;
					int32 InPinIndex = NextNode->Pins
						.FilterByPredicate([](UEdGraphPin* Pin) {
						return Pin->Direction == EEdGraphPinDirection::EGPD_Input;
					})
						.IndexOfByPredicate([LinkedTo](UEdGraphPin* Pin) {
						return Pin == LinkedTo;
					});
					check(InPinIndex != -1);

					ComputeFlow->ConnectNodes(OutComputeNode, OutPinIndex, InComputeNode, InPinIndex);
				}
			} // End Every In Pin
		} // End Every Out Pin
	} // End every node
}

void UFroxComputeFlowComponent::InitializeFlowInputs(const TArray<UInputPropertyNode*>& Inputs, const TArray<NodePair>& Pairs)
{
	check(ComputeFlow)

	// Set inputs
	for (UInputPropertyNode* Input : Inputs)
	{
		// uint32_t entryId = ComputeFlow->CreateEntry();

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
					const NodePair* InPair = Pairs.FindByPredicate([NextNode](NodePair Pair) {
						return Pair.Key == NextNode;
					});

					if (InPair)
					{
						frox::ComputeNode* InComputeNode = InPair->Value;

						int32 InPinIndex = NextNode->Pins
							.FilterByPredicate([](UEdGraphPin* Pin) {
							return Pin->Direction == EEdGraphPinDirection::EGPD_Input;
						})
							.IndexOfByPredicate([LinkedTo](UEdGraphPin* Pin) {
							return Pin == LinkedTo;
						});
						check(InPinIndex != -1);

						// Create Input By Prop
						auto FoundInput = InputHoles.Find(Input->PropertyName);
						FHole& InputHole = !FoundInput ? InputHoles.Add(Input->PropertyName, FHole{}) : *FoundInput;
						InputHole.Nodes.Add(FHoleNode{ InComputeNode, uint32(InPinIndex) });
					}
				}
			} // End Every In Pin
		} // End Every Out Pin
	} // End every input
}

void UFroxComputeFlowComponent::InitializeFlowOutputs(const TArray<UOutputPropertyNode*>& Outputs, const TArray<NodePair>& Pairs)
{
	// Set outputs
	for (UOutputPropertyNode* Output : Outputs)
	{
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
					const NodePair* OutPair = Pairs.FindByPredicate([NextNode](NodePair Pair) {
						return Pair.Key == NextNode;
					});

					if (OutPair)
					{
						frox::ComputeNode* OutComputeNode = OutPair->Value;
						int32 OutPinIndex = NextNode->Pins
							.FilterByPredicate([](UEdGraphPin* Pin) {
							return Pin->Direction == EEdGraphPinDirection::EGPD_Output;
						})
							.IndexOfByPredicate([LinkedTo](UEdGraphPin* Pin) {
							return Pin == LinkedTo;
						});
						check(OutPinIndex != -1);

						frox::ComputeFramePtr frame = OutComputeNode->GetOutput(OutPinIndex);

						// Add hole
						auto FoundIOutput = OuputHoles.Find(Output->PropertyName);
						FHole& OuputHole = !FoundIOutput ? OuputHoles.Add(Output->PropertyName, FHole{}) : *FoundIOutput;
						OuputHole.Nodes.Add(FHoleNode{ OutComputeNode, uint32(OutPinIndex) });
					}
				}
			} // End Every Out Pin
		} // End Every In Pin
	} // End every output
}

void UFroxComputeFlowComponent::ReleaseFlow()
{
	if (ComputeFlow)
	{
		frox::Frox* FroxLib = FFroxPluginModule::Get().GetFrox();
		check(FroxLib != nullptr);

		FroxLib->DestroyComputeFlow(ComputeFlow);
	}
}

void UFroxComputeFlowComponent::TickFlow()
{
	check(ComputeFlow != nullptr)

	// Wait
	ComputeFlow->Fetch();

	// Get Outputs
	for (const FComputeFlowEntry& entry : ComputeFlowAsset->Keys)
	{
		auto FoundOuput = OuputHoles.Find(entry.EntryName);
		if (FoundOuput)
		{
			const FHole& Ouput = *FoundOuput;
			for (const FHoleNode& Node : Ouput.Nodes)
			{
				frox::ComputeFramePtr FroxFrame = Node.Node->GetOutput(Node.PinId);
				if (FroxFrame)
				{
					UFroxComputeFrame* ComputeFrame = GetValueAsFrame(entry.EntryName);
					if (ComputeFrame != nullptr)
					{
						ComputeFrame->SetFroxFrame(FroxFrame);
						SetValueAsFrame(entry.EntryName, ComputeFrame);
					}
					else
					{
						ComputeFrame = NewObject<UFroxComputeFrame>();
						ComputeFrame->SetFroxFrame(FroxFrame);
						SetValueAsFrame(entry.EntryName, ComputeFrame);
					}
				}
			}
		} // End input
	} // End every key

	// Update Inputs
	for (const FComputeFlowEntry& entry : ComputeFlowAsset->Keys)
	{
		auto FoundInput = InputHoles.Find(entry.EntryName);
		if (FoundInput)
		{
			const FHole& Input = *FoundInput;
			UFroxComputeFrame* ComputeFrame = GetValueAsFrame(entry.EntryName);
			if (ComputeFrame != nullptr)
			{
				frox::ComputeFramePtr FroxFrame = ComputeFrame->GetFroxFrame();
				for (const FHoleNode& Node : Input.Nodes)
				{
					Node.Node->SetInput(Node.PinId, FroxFrame);
				}
			}
		} // End input
	} // End every key

	// Push Calculation
	ComputeFlow->Perform();
}

#undef LOCTEXT_NAMESPACE