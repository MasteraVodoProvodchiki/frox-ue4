#include "FroxNodeBase.h"

const FName UFroxNodeBase::PC_Frame(TEXT("frame"));
const FName UFroxNodeBase::PC_Property(TEXT("property"));
const FName UFroxNodeBase::PC_Any(TEXT("any"));

#if WITH_EDITORONLY_DATA
void UFroxNodeBase::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (PropertyObserver.IsValid())
	{
		FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
		PropertyObserver->OnPropertyChanged(this, PropertyName);
	}

	Super::PostEditChangeProperty(e);
}

bool UFroxNodeBase::CycleInspection(uint32 UniqueID)
{
	for (int i = 0; i < Pins.Num(); i++)
	{
		if (Pins[i]->Direction == EEdGraphPinDirection::EGPD_Output)
		{
			for (int j = 0; j < Pins[i]->LinkedTo.Num(); j++)
			{
				if (Pins[i]->LinkedTo[j])
				{
					if (Pins[i]->LinkedTo[j]->GetOwningNode()->GetUniqueID() == UniqueID)
					{
						return false;
					}
					else
					{
						if (!Cast<UFroxNodeBase>(Pins[i]->LinkedTo[j]->GetOwningNode())->CycleInspection(UniqueID))
						{
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

bool UFroxNodeBase::ReallocatePins()
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
#endif //WITH_EDITORONLY_DATA

void UFroxNodeBase::MarkNodeRequiresSynchronization(FString Reason, bool bRaiseGraphNeedsRecompile)
{
	Modify();
	ChangeId = FGuid::NewGuid();

	VisualsChangedDelegate.Broadcast(this);
}

UFroxNodeBase::FOnNodeVisualsChanged& UFroxNodeBase::OnVisualsChanged()
{
	return VisualsChangedDelegate;
}