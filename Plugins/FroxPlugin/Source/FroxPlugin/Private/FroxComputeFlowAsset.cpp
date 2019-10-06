#include "FroxComputeFlowAsset.h"
#include "Shared.h"
#include "FroxPlugin.h"
#include "Nodes/FroxNods.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"

#include "Frox/Frox/Frox.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/ComputeFrame.h"
#include "Frox/Frox/ComputeNode.h"

#define LOCTEXT_NAMESPACE "FroxComputeFlowAsset"

bool FComputeFlowEntry::operator==(const FComputeFlowEntry& Other) const
{
	return (EntryName == Other.EntryName) && (
		KeyType == Other.KeyType &&
		Direction == Other.Direction
		);
}

frox::EPinValueType FlowKeyTypeToPionType(EComputeFlowKeyType KeyType)
{
	if (KeyType == EComputeFlowKeyType::ECFKT_Frame)
	{
		return frox::EPinValueType::Frame;
	}
	if (KeyType == EComputeFlowKeyType::ECFKT_Data)
	{
		return frox::EPinValueType::Data;
	}
	
	return frox::EPinValueType::Value;
}


FName UFroxComputeFlowAsset::GetComputeFlowName() const
{
	return computeFlowFileName;
}

bool UFroxComputeFlowAsset::FindEntry(FName EntryName, FComputeFlowEntry& Out) const
{
	const FComputeFlowEntry* Found = Keys.FindByPredicate([EntryName](const FComputeFlowEntry& Entry) {
		return Entry.EntryName == EntryName;
	});

	if (Found != nullptr)
	{
		Out = *Found;
		return true;
	}

	return false;
}

#if WITH_EDITORONLY_DATA
void UFroxComputeFlowAsset::SetComputeFlowName(const FName& ComputeFlowFileName)
{
	computeFlowFileName = ComputeFlowFileName;
}

void UFroxComputeFlowAsset::PostInitProperties()
{
	// if (!HasAnyFlags(RF_ClassDefaultObject)) importData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	Super::PostInitProperties();
}

void UFroxComputeFlowAsset::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	//if (importData) {
	//	OutTags.Add(FAssetRegistryTag(SourceFileTagName(), importData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden) );
	//}

	Super::GetAssetRegistryTags(OutTags);
}

void UFroxComputeFlowAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	//if (Ar.IsLoading() && Ar.UE4Ver() < VER_UE4_ASSET_IMPORT_DATA_AS_JSON && !importData)
	//	importData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
}

void UFroxComputeFlowAsset::PostLoad()
{
	for (FComputeFlowEntry& Key : Keys)
	{
		if (!Key.UniqueId.IsValid())
		{
			Key.UniqueId = FGuid::NewGuid();
		}
	}
	
	Super::PostLoad();
}

#endif

void UFroxComputeFlowAsset::BeginDestroy() {

	Super::BeginDestroy();
}

frox::ComputeFlow* UFroxComputeFlowAsset::CreateFlow() const
{
	check(UpdateGraph != nullptr);

	// Create ComputeFlow by FlowDesc
	frox::Frox* FroxLib = IFroxPlugin::Get().GetFrox();
	check(FroxLib != nullptr);

	frox::ComputeFlow* ComputeFlow = FroxLib->CreateComputeFlow();
	check(ComputeFlow != nullptr);


	// Group
	TArray<UPropertyNode*> Properties;
	UInputPropertyNode* InputNode = nullptr;
	UOutputPropertyNode* OutputNode = nullptr;
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
		UPropertyNode* PropertyGraphNode = Cast<UPropertyNode>(Node);
		if (PropertyGraphNode)
		{
			Properties.Add(PropertyGraphNode);
			continue;
		}

		// Input
		UInputPropertyNode* InputGraphNode = Cast<UInputPropertyNode>(Node);
		if (InputGraphNode)
		{
			InputNode = InputGraphNode;
			continue;
		}

		// Output
		UOutputPropertyNode* OutputGraphNode = Cast<UOutputPropertyNode>(Node);
		if (OutputGraphNode)
		{
			OutputNode = OutputGraphNode;
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

	InitializeFlowOperations(ComputeFlow, Operations, Pairs);
	InitializeFlowInputs(ComputeFlow, InputNode, Pairs);
	InitializeFlowOutputs(ComputeFlow, OutputNode, Pairs);

	return ComputeFlow;
}

void UFroxComputeFlowAsset::InitializeFlowOperations(frox::ComputeFlow* ComputeFlow, const TArray<UOpartionNode*>& Operations, const TArray<NodePair>& Pairs) const
{
	check(ComputeFlow)

	// Connect operations
	for (const NodePair& OutPair : Pairs)
	{
		UOpartionNode* OperationGraphNode = OutPair.Key;
		frox::ComputeNode* OutComputeNode = OutPair.Value;

		// Only for output
		TArray<UEdGraphPin*> OutputPins = OperationGraphNode->Pins.FilterByPredicate([](UEdGraphPin* Pin) {
			return Pin->Direction == EEdGraphPinDirection::EGPD_Output; // && Pin->PinType.PinCategory == UFroxNodeBase::PC_Frame;
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
							return Pin->Direction == EEdGraphPinDirection::EGPD_Input; // && Pin->PinType.PinCategory == UFroxNodeBase::PC_Frame;
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

void UFroxComputeFlowAsset::InitializeFlowInputs(frox::ComputeFlow* ComputeFlow, const UInputPropertyNode* InputNode, const TArray<NodePair>& Pairs) const
{
	check(ComputeFlow);
	check(InputNode);

	TMap<FGuid, UEdGraphPin*> KeySet;
	TArray<UEdGraphPin*> OutputPins = InputNode->Pins.FilterByPredicate([](UEdGraphPin* Pin) {
		return Pin->Direction == EEdGraphPinDirection::EGPD_Output; // && Pin->PinType.PinCategory == UFroxNodeBase::PC_Frame;
	});
	for (UEdGraphPin* Pin : OutputPins)
	{
		if (Pin->LinkedTo.Num())
		{
			KeySet.Add(Pin->PinId, Pin);
		}
	}

	for (FComputeFlowEntry Entry : Keys)
	{
		if (Entry.Direction == EComputeFlowEntryDirection::ECFED_Input && KeySet.Contains(Entry.UniqueId))
		{
			UEdGraphPin* Pin = KeySet[Entry.UniqueId];
			KeySet.Remove(Entry.UniqueId);
			frox::EPinValueType PinValueType = FlowKeyTypeToPionType(Entry.KeyType);
			ANSICHAR EntryName[1024];
			Entry.EntryName.GetPlainANSIString(EntryName);
			uint32_t EntryId = ComputeFlow->FindOrCreateEntry(EntryName, PinValueType);
			// Only for output
			// for (UEdGraphPin* Pin : OutputPins)
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
									return Pin->Direction == EEdGraphPinDirection::EGPD_Input; // && Pin->PinType.PinCategory == UFroxNodeBase::PC_Frame;
								})
								.IndexOfByPredicate([LinkedTo](UEdGraphPin* Pin) {
									return Pin == LinkedTo;
								});
							check(InPinIndex != -1);

							// Create Input By Prop
							ComputeFlow->ConnectEntry(EntryId, InComputeNode, InPinIndex);
						}
					}
				} // End Every In Pin
			} // End Every Out Pin
		}
	}

	// Log Pins that are missing in Keys.
	for (auto& Pair : KeySet)
	{
		UE_LOG(LogFrox, Error, TEXT("Not found Entry '%s'. Check Inputs!"), *Pair.Value->GetName());
	} // End every input
}

void UFroxComputeFlowAsset::InitializeFlowOutputs(frox::ComputeFlow* ComputeFlow, const UOutputPropertyNode* OutputNode, const TArray<NodePair>& Pairs) const
{
	TMap<FGuid, UEdGraphPin*> KeySet;
	TArray<UEdGraphPin*> InputPins = OutputNode->Pins.FilterByPredicate([](UEdGraphPin* Pin) {
		return Pin->Direction == EEdGraphPinDirection::EGPD_Input; // && Pin->PinType.PinCategory == UFroxNodeBase::PC_Frame;
	});
	for (UEdGraphPin* Pin : InputPins)
	{
		if (Pin->LinkedTo.Num())
		{
			KeySet.Add(Pin->PinId, Pin);
		}
	}

	for (FComputeFlowEntry Entry : Keys)
	{
		if (Entry.Direction == EComputeFlowEntryDirection::ECFED_Output && KeySet.Contains(Entry.UniqueId))
		{
			UEdGraphPin* Pin = KeySet[Entry.UniqueId];
			KeySet.Remove(Entry.UniqueId);

			frox::EPinValueType PinValueType = Entry.KeyType == EComputeFlowKeyType::ECFKT_Frame ?
				frox::EPinValueType::Frame :
				frox::EPinValueType::Value;
			ANSICHAR EntryName[1024];
			Entry.EntryName.GetPlainANSIString(EntryName);
			uint32_t OutputId = ComputeFlow->FindOrCreateOutput(EntryName, PinValueType);

			// Only for inputs
			// for (UEdGraphPin* Pin : InputPins)
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
									return Pin->Direction == EEdGraphPinDirection::EGPD_Output; // && Pin->PinType.PinCategory == UFroxNodeBase::PC_Frame;
								})
								.IndexOfByPredicate([LinkedTo](UEdGraphPin* Pin) {
									return Pin == LinkedTo;
								});
							check(OutPinIndex != -1);

							ComputeFlow->ConnectOutput(OutputId, OutComputeNode, OutPinIndex);
						}
					}
				} // End Every Out Pin
			} // End Every In Pin
		}
	}

	// Log Pins that are missing in Keys.
	for (auto& Pair : KeySet)
	{
		UE_LOG(LogFrox, Error, TEXT("Not found Entry '%s'. Check Inputs!"), *Pair.Value->GetName());
	} // End every input
}

#undef LOCTEXT_NAMESPACE