#include "FroxCropNods.h"
#include "FroxPlugin.h"
#include "FroxTypes.h"

#include "Frox/Frox/ComputeFlow.h"
#include "Frox/Frox/CropComputeNode.h"

#define LOCTEXT_NAMESPACE "UCropNods"

/// Crop
#if WITH_EDITORONLY_DATA
void UCropOpartionNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	UEdGraphPin* OffsetPin = CreatePin(EGPD_Input, UFroxNodeBase::PC_Property, FFroxTypeDefinition::IntType, TEXT("Offset"));
	OffsetPin->DefaultValue = TEXT("0, 0");
	OffsetPin->bAdvancedView = true;

	UEdGraphPin* SizePin = CreatePin(EGPD_Input, UFroxNodeBase::PC_Property, FFroxTypeDefinition::IntType, TEXT("Size"));
	SizePin->DefaultValue = TEXT("1, 1");
	SizePin->bAdvancedView = true;

	/*
	UEdGraphPin* XPin = CreatePin(EGPD_Input, UFroxNodeBase::PC_Property, FFroxTypeDefinition::IntType, TEXT("X"));
	XPin->DefaultValue = TEXT("0");
	XPin->bAdvancedView = true;
	
	UEdGraphPin* YPin = CreatePin(EGPD_Input, UFroxNodeBase::PC_Property, FFroxTypeDefinition::IntType, TEXT("Y"));
	XPin->DefaultValue = TEXT("0");
	YPin->bAdvancedView = true;

	UEdGraphPin* WidthPin = CreatePin(EGPD_Input, UFroxNodeBase::PC_Property, FFroxTypeDefinition::IntType, TEXT("Width"));
	WidthPin->DefaultValue = TEXT("1");
	WidthPin->bAdvancedView = true;

	UEdGraphPin* HeightPin = CreatePin(EGPD_Input, UFroxNodeBase::PC_Property, FFroxTypeDefinition::IntType, TEXT("Height"));
	HeightPin->DefaultValue = TEXT("1");
	HeightPin->bAdvancedView = true;
	*/
	if (ENodeAdvancedPins::NoPins == AdvancedPinDisplay)
	{
		AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	}
}
#endif

frox::ComputeNode* UCropOpartionNode::CreateFroxNode(frox::ComputeFlow* Flow) const
{
	check(Flow != nullptr);

	auto CropNode = Flow->CreateNode<frox::CropComputeNode>();
	check(CropNode != nullptr);

	CropNode->SetOffset(frox::Point{ Offset.X, Offset.Y });
	CropNode->SetSize(frox::Size{ uint32(Size.X), uint32(Size.Y) });

	return CropNode;
}

#undef LOCTEXT_NAMESPACE