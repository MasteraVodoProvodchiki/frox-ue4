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

#if WITH_EDITORONLY_DATA
void UInputPropertyNode::AllocateDefaultPins()
{
	UEdGraphPin* Output = CreatePin(EGPD_Output, TEXT(""), TEXT("Out"));
}

FText UInputPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("InputProperty");
}

void UOutputPropertyNode::AllocateDefaultPins()
{
	UEdGraphPin* In = CreatePin(EGPD_Input, TEXT(""), TEXT("In"));
}

FText UOutputPropertyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("OutputProperty");
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
	return FText::FromString(this->GetTypeName());
}
#endif //WITH_EDITORONLY_DATA

frox::ComputeNode* UMakeZeroFrameNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);
	return Flow->CreateNode(this->GetTypeName());
	// set w/h
}

#if WITH_EDITORONLY_DATA
void UMakeZeroFrameNode::AllocateDefaultPins()
{
	UEdGraphPin* Width = CreatePin(EGPD_Input, TEXT(""), TEXT("Width"));
	Width->DefaultValue = "1";
	UEdGraphPin* Height = CreatePin(EGPD_Input, TEXT(""), TEXT("Height"));
	Height->DefaultValue = "1";

	UEdGraphPin* Output = CreatePin(EGPD_Output, TEXT(""), TEXT("Out"));
}

FText UMakeZeroFrameNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTypeName());
}
#endif //WITH_EDITORONLY_DATA