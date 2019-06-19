#include "FroxNods.h"
#include "OperationLogic.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"

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

#if WITH_EDITORONLY_DATA
void UInputPropertyNode::AllocateDefaultPins()
{
	UEdGraphPin* Output = CreatePin(EGPD_Output, TEXT(""), TEXT("Out"));
}

FText UInputPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("InputProperty");
}

FLinearColor UInputPropertyNode::GetNodeTitleColor() const
{
	return FLinearColor(11.f / 255.f, 102.f / 255.f, 35.f / 255.f);
}

void UOutputPropertyNode::AllocateDefaultPins()
{
	UEdGraphPin* In = CreatePin(EGPD_Input, TEXT(""), TEXT("In"));
}

FText UOutputPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("OutputProperty");
}

FLinearColor UOutputPropertyNode::GetNodeTitleColor() const
{
	return FLinearColor(11.f / 255.f, 102.f / 255.f, 35.f / 255.f);
}
#endif //WITH_EDITORONLY_DATA

frox::ComputeNode* UOpartionNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	return Flow->CreateNode(this->GetTypeName());
}

#if WITH_EDITORONLY_DATA
void UOpartionNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	UFroxNodeBase::PostEditChangeProperty(e);
}

FText UOpartionNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}
#endif //WITH_EDITORONLY_DATA