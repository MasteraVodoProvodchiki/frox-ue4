#include "FroxNods.h"
#include "OperationLogic.h"

#include "EdGraph/EdGraphPin.h"
#include "Frox/Frox/ComputeFlow.h"

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
	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Any, TEXT("Out"));
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
	UEdGraphPin* In = CreatePin(EGPD_Input, UFroxNodeBase::PC_Any, TEXT("In"));
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