#include "Nodes/FroxArithmeticNods.h"
#include "EdGraph/EdGraphPin.h"

#if WITH_EDITORONLY_DATA
FText UOpartionBasicNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTitle());
}

void UOpartionBasicNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	UFroxNodeBase::PostEditChangeProperty(e);
}
#endif //WITH_EDITORONLY_DATA

#if WITH_EDITORONLY_DATA
void UOpartionInput1OuputNode::AllocateDefaultPins()
{
	UEdGraphPin* In = CreatePin(EGPD_Input, UFroxNodeBase::PC_Frame, TEXT("In"));
	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Frame, TEXT("Out"));
}
#endif //WITH_EDITORONLY_DATA

#if WITH_EDITORONLY_DATA
void UOpartionInput2OuputNode::AllocateDefaultPins()
{
	UEdGraphPin* Left = CreatePin(EGPD_Input, UFroxNodeBase::PC_Frame, TEXT("Left"));
	UEdGraphPin* Right = CreatePin(EGPD_Input, UFroxNodeBase::PC_Frame, TEXT("Right"));
	UEdGraphPin* Output = CreatePin(EGPD_Output, UFroxNodeBase::PC_Frame, TEXT("Out"));
}
#endif //WITH_EDITORONLY_DATA