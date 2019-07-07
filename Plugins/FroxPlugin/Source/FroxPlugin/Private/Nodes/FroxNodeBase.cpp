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