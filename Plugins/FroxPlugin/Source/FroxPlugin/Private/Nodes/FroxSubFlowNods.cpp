#include "Nodes/FroxSubFlowNods.h"
#include "FroxPlugin.h"
#include "FroxComputeFlowAsset.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/SubFlowComputeNode.h"

#define LOCTEXT_NAMESPACE "UMakeFrameNode"

frox::ComputeNode* USubFlowNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto SubFlowNode = Flow->CreateNode<frox::SubFlowComputeNode>();

	return SubFlowNode;
}

void USubFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(USubFlowNode, ComputeFlow))
	{
		ReallocatePins();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

#if WITH_EDITORONLY_DATA
bool USubFlowNode::ReallocatePins()
{
	Modify();

	// Move the existing pins to a saved array
	TArray<UEdGraphPin*> OldPins(Pins);
	Pins.Reset();

	// Recreate the new pins
	AllocateDefaultPins();

	// Determine if the pins are the same as they were previously...
	bool bAllSame = OldPins.Num() == Pins.Num();

	// Copy the old pin data and remove it.
	for (int32 OldPinIndex = 0; OldPinIndex < OldPins.Num(); ++OldPinIndex)
	{
		UEdGraphPin* OldPin = OldPins[OldPinIndex];

		// When matching pins, use the pin id if either pin id is valid, otherwise match by name.
		auto PinMatchPredicate = [&](UEdGraphPin* Pin)
		{
			bool bPinsHaveIds = Pin->PersistentGuid.IsValid() || OldPin->PersistentGuid.IsValid();
			return Pin->Direction == OldPin->Direction && ((bPinsHaveIds && Pin->PersistentGuid == OldPin->PersistentGuid) || (bPinsHaveIds == false && Pin->PinName == OldPin->PinName));
		};

		OldPin->Modify();
		if (UEdGraphPin** MatchingNewPin = Pins.FindByPredicate(PinMatchPredicate))
		{
			// If the pin types don't match, CopyPersistentDataFromOldPin could very well overwrite our Matching pin with bad data.
			// Let's cache it for now and reset it after copying the other relevant data off the old pin.
			FString DefaultValue;
			FString AutogeneratedDefaultValue;
			class UObject* DefaultObject = nullptr;
			FText DefaultTextValue;
			bool bTypeMismatch = (*MatchingNewPin)->PinType != OldPin->PinType;

			if (bTypeMismatch)
			{
				DefaultValue = (*MatchingNewPin)->DefaultValue;
				DefaultObject = (*MatchingNewPin)->DefaultObject;
				DefaultTextValue = (*MatchingNewPin)->DefaultTextValue;
			}

			// This copies the existing default values, pin linkages, advanced pin view, pin splitting, etc.
			(*MatchingNewPin)->MovePersistentDataFromOldPin(*OldPin);

			if (bTypeMismatch)
			{
				(*MatchingNewPin)->DefaultValue = DefaultValue;
				(*MatchingNewPin)->DefaultObject = DefaultObject;
				(*MatchingNewPin)->DefaultTextValue = DefaultTextValue;
				bAllSame = false;
			}
		}
		else
		{
			bAllSame = false;
		}

		OldPin->MarkPendingKill();
	}

	if (!bAllSame)
	{
		MarkNodeRequiresSynchronization(__FUNCTION__, true);
	}
	else
	{
		VisualsChangedDelegate.Broadcast(this);
	}
	return bAllSame;
}

void USubFlowNode::AllocateDefaultPins()
{
	if (ComputeFlow)
	{
		for (const FComputeFlowEntry& entry : ComputeFlow->Keys)
		{
			FString PinName = entry.EntryName.ToString();
			if (entry.Direction == EComputeFlowEntryDirection::ECFED_Input)
			{			
				CreatePin(EGPD_Input, TEXT(""), *PinName);
			}
			else if (entry.Direction == EComputeFlowEntryDirection::ECFED_Output)
			{
				CreatePin(EGPD_Output, TEXT(""), *PinName);
			}
		}
	}
}

FText USubFlowNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTypeName());
}

FLinearColor USubFlowNode::GetNodeTitleColor() const
{
	return FLinearColor(0.f, 0.5f, 1.f);
}
#endif //WITH_EDITORONLY_DATA

#undef LOCTEXT_NAMESPACE