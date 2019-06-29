#include "FroxNods.h"
#include "OperationLogic.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"

const FName UFroxNodeBase::PC_Frame(TEXT("frame"));
const FName UFroxNodeBase::PC_Property(TEXT("property"));

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


/// Property
void UPropertyNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName ChangedPropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UPropertyNode, PropertyName))
	{
		//
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

#if WITH_EDITORONLY_DATA
void UPropertyNode::AllocateDefaultPins()
{
	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Property, TEXT("Out"));
}

FText UPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return PropertyName.IsValid() ? FText::FromName(PropertyName) : FText::FromString("Property");
}

FLinearColor UPropertyNode::GetNodeTitleColor() const
{
	return FLinearColor(108.f / 255.f, 52.f / 255.f, 131.f / 255.f);
}
#endif //WITH_EDITORONLY_DATA


void UInputPropertyNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName ChangedPropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UPropertyNode, PropertyName))
	{
		//
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

#if WITH_EDITORONLY_DATA
void UInputPropertyNode::AllocateDefaultPins()
{
	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Frame, TEXT("Out"));
}

FText UInputPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return PropertyName.IsValid() ? FText::FromName(PropertyName) : FText::FromString("InputFrame");
}

FLinearColor UInputPropertyNode::GetNodeTitleColor() const
{
	return FLinearColor(11.f / 255.f, 102.f / 255.f, 35.f / 255.f);
}
#endif

void UOutputPropertyNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName ChangedPropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UPropertyNode, PropertyName))
	{
		//
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
}

#if WITH_EDITORONLY_DATA
void UOutputPropertyNode::AllocateDefaultPins()
{
	UEdGraphPin* In = CreatePin(EGPD_Input, UFroxNodeBase::PC_Frame, TEXT("In"));
}

FText UOutputPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return PropertyName.IsValid() ? FText::FromName(PropertyName) : FText::FromString("OutputFrame");
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