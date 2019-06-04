#include "FroxNods.h"
#include "OperationLogic.h"

#include "EdGraph/EdGraphPin.h"


const FString FPinDataTypes::PinType_Root = "root";


//UCustomNodeBase...........................................................................................

TArray<UFroxNodeBase*> UFroxNodeBase::GetChildNodes(FRandomStream& RandomStream)
{
	TArray<UFroxNodeBase*> ChildNodes;
	return ChildNodes;
}

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

const char* UOpartionNode::GetTypeName() const
{
	return OperationInstance ? OperationInstance->GetTypeName() : "none";
}

#if WITH_EDITORONLY_DATA
void UOpartionNode::AllocateDefaultPins()
{
	UEdGraphPin* Left = CreatePin(EGPD_Input, FPinDataTypes::PinType_Root, TEXT(""), NULL, false, false, TEXT("Left"));
	UEdGraphPin* Right = CreatePin(EGPD_Input, FPinDataTypes::PinType_Root, TEXT(""), NULL, false, false, TEXT("Right"));
	UEdGraphPin* Output = CreatePin(EGPD_Output, FPinDataTypes::PinType_Root, TEXT(""), NULL, false, false, TEXT("Out"));
}

void UOpartionNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (Operation) OperationInstance = NewObject<UOperationLogic>(this, Operation);
	UFroxNodeBase::PostEditChangeProperty(e);
}
FText UOpartionNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (Operation) return FText::FromString(Operation->GetName());
	return FText::FromString("Not Rule");
}
#endif //WITH_EDITORONLY_DATA

TArray<UFroxNodeBase*> UOpartionNode::GetChildNodes(FRandomStream& RandomStream)
{
	UEdGraphPin* OutPin = Pins[1];
	TArray<UFroxNodeBase*> ChildNodes;

	if (!Operation)
	{
		return ChildNodes;
	}
	else
	{
		if (!OperationInstance)
		{
			OperationInstance = NewObject<UOperationLogic>(this, Operation);
		}
	}
	if (OperationInstance->NodeInvoke(RandomStream))
	{
		for (int i = 0; i < OutPin->LinkedTo.Num(); i++)
		{
			if (OutPin->LinkedTo[i])
			{
				ChildNodes.Add((UFroxNodeBase*)OutPin->LinkedTo[i]->GetOwningNode());
			}
		}
	}
	return ChildNodes;
}
