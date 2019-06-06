#include "FroxArithmeticNods.h"
#include "EdGraph/EdGraphPin.h"

#if WITH_EDITORONLY_DATA
void UOpartionInput2OuputNode::AllocateDefaultPins()
{
	UEdGraphPin* Left = CreatePin(EGPD_Input, FPinDataTypes::PinType_Root, TEXT(""), NULL, false, false, TEXT("Left"));
	UEdGraphPin* Right = CreatePin(EGPD_Input, FPinDataTypes::PinType_Root, TEXT(""), NULL, false, false, TEXT("Right"));
	UEdGraphPin* Output = CreatePin(EGPD_Output, FPinDataTypes::PinType_Root, TEXT(""), NULL, false, false, TEXT("Out"));
}

void UOpartionInput2OuputNode::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	UFroxNodeBase::PostEditChangeProperty(e);
}

FText UOpartionInput2OuputNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetTypeName());
}
#endif //WITH_EDITORONLY_DATA