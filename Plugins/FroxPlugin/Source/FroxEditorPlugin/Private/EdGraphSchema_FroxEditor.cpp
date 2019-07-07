#include "EdGraphSchema_FroxEditor.h"

#include "FroxTypes.h"
#include "Nodes/FroxNods.h"
#include "Nodes/FroxArithmeticNods.h"
#include "Nodes/FroxMakeNods.h"
#include "Nodes/FroxConvertNods.h"
#include "Nodes/FroxCropNods.h"
#include "Nodes/FroxSubFlowNods.h"
#include "Nodes/FroxConstNods.h"
#include "Nodes/FroxFrameNods.h"
#include "Nodes/UE4CustomNods.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "GraphEditorSettings.h"
#include "GenericCommands.h"
#include "GraphEditorActions.h"
#include "FroxConnectionDrawingPolicy.h"


#define SNAP_GRID (16) // @todo ensure this is the same as SNodePanel::GetSnapGridSize()


namespace
{
	// Maximum distance a drag can be off a node edge to require 'push off' from node
	const int32 NodeDistance = 60;
}

UEdGraphNode* FFroxSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	UEdGraphNode* ResultNode = NULL;

	// If there is a template, we actually use it
	if (NodeTemplate != NULL)
	{
		NodeTemplate->SetFlags(RF_Transactional);

		// set outer to be the graph so it doesn't go away
		NodeTemplate->Rename(NULL, (UObject *)ParentGraph, REN_NonTransactional);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);


		// For input pins, new node will generally overlap node being dragged off
		// Work out if we want to visually push away from connected node
		int32 XLocation = Location.X;
		if (FromPin && FromPin->Direction == EGPD_Input)
		{
			UEdGraphNode* PinNode = FromPin->GetOwningNode();
			const float XDelta = FMath::Abs(PinNode->NodePosX - Location.X);

			if (XDelta < NodeDistance)
			{
				// Set location to edge of current node minus the max move distance
				// to force node to push off from connect node enough to give selection handle
				XLocation = PinNode->NodePosX - NodeDistance;
			}
		}

		NodeTemplate->NodePosX = XLocation;
		NodeTemplate->NodePosY = Location.Y;
		NodeTemplate->SnapToGrid(SNAP_GRID);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

UEdGraphNode* FFroxSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	UEdGraphNode* ResultNode = NULL;

	if (FromPins.Num() > 0)
	{
		ResultNode = PerformAction(ParentGraph, FromPins[0], Location, bSelectNewNode);

		// Try autowiring the rest of the pins
		for (int32 Index = 1; Index < FromPins.Num(); ++Index)
		{
			ResultNode->AutowireNewNode(FromPins[Index]);
		}
	}
	else
	{
		ResultNode = PerformAction(ParentGraph, NULL, Location, bSelectNewNode);
	}

	return ResultNode;
}

void FFroxSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);

	// These don't get saved to disk, but we want to make sure the objects don't get GC'd while the action array is around
	Collector.AddReferencedObject(NodeTemplate);
}
FString Combine(const TArray<FString> Array, FString Separator)
{
	// TODO: Use a string builder
	FString Result;
	for (const FString& Item : Array) {
		if (Result.Len() > 0) {
			Result += Separator;
		}
		Result += Item;
	}
	return Result;
}

UEdGraphSchema_FroxEditor::UEdGraphSchema_FroxEditor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEdGraphSchema_FroxEditor::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	FFormatNamedArguments Args;
	const FName AttrName("Attributes");
	Args.Add(TEXT("Attribute"), FText::FromName(AttrName));
	const UEdGraphPin* FromPin = ContextMenuBuilder.FromPin;
	const UEdGraph* Graph = ContextMenuBuilder.CurrentGraph;
	TArray<TSharedPtr<FEdGraphSchemaAction> > Actions;

	FroxSchemaUtils::AddAction<UPropertyNode>(TEXT("Add Property"), TEXT("Add input property node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UInputPropertyNode>(TEXT("Add Input Frame"), TEXT("Add input frame node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UOutputPropertyNode>(TEXT("Add Output Frame"), TEXT("Add output frame node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	
	FroxSchemaUtils::AddAction<UMakeFrameNode>(TEXT("Add Make Node"), TEXT("Add make node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UMakeZeroFrameNode>(TEXT("Add Make Zero Node"), TEXT("Add make zero node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UMakeNoiseFrameNode>(TEXT("Add Make Noise Node"), TEXT("Add make noise node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UUEImageReaderNode>(TEXT("Add UE4 Img Read Node"), TEXT("Add ue4 img reader node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UConstFrameNode>(TEXT("Add Const Frame Node"), TEXT("Add const frame node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);

	FroxSchemaUtils::AddAction<UAddOpartionNode>(TEXT("Add Add Operation Node"), TEXT("Add Add operation node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<USubOpartionNode>(TEXT("Add Sub Operation Node"), TEXT("Add Sub operation node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UMulOpartionNode>(TEXT("Add Mul Operation Node"), TEXT("Add Mul operation node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UDivOpartionNode>(TEXT("Add Div Operation Node"), TEXT("Add Div operation node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UAvgOpartionNode>(TEXT("Add Avg Operation Node"), TEXT("Add Avg operation node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);

	FroxSchemaUtils::AddAction<UConvertToOpartionNode>(TEXT("Add ConverTo Operation Node"), TEXT("Add ConverTo operation node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UCropOpartionNode>(TEXT("Add Crop Operation Node"), TEXT("Add Crop operation node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<UFrameSizeOpartionNode>(TEXT("Add FramSize Node"), TEXT("Add FrameSize node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	FroxSchemaUtils::AddAction<USubFlowNode>(TEXT("Add SubFlow Operation Node"), TEXT("Add SubFlow operation node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);

	for (TSharedPtr<FEdGraphSchemaAction> Action : Actions)
	{
		ContextMenuBuilder.AddAction(Action);
	}
}

const FPinConnectionResponse UEdGraphSchema_FroxEditor::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	UFroxNodeBase* ABase = Cast<UFroxNodeBase>(A->GetOwningNode());
	UFroxNodeBase* BBase = Cast<UFroxNodeBase>(B->GetOwningNode());

	// Make sure the input is connecting to an output
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed"));
	}

	// Make sure the data types match
	if (A->PinType.PinCategory != B->PinType.PinCategory)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed"));
	}

	//Make sure not same node
	if (ABase->GetUniqueID() == BBase->GetUniqueID())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Same node"));
	}

	//Make sure not loop
	if (!BBase->CycleInspection(ABase->GetUniqueID()))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Create loop"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

class FConnectionDrawingPolicy* UEdGraphSchema_FroxEditor::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	// return new FFroxConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	return new FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}

/*
void UEdGraphSchema_FroxEditor::GetPinDefaultValuesFromString(const FEdGraphPinType& PinType, UObject* OwningObject, const FString& NewDefaultValue, FString& UseDefaultValue, UObject*& UseDefaultObject, FText& UseDefaultText) const
{

}
*/

void UEdGraphSchema_FroxEditor::TrySetDefaultValue(UEdGraphPin& InPin, const FString& NewDefaultValue) const
{
	Super::TrySetDefaultValue(InPin, NewDefaultValue);
}

void UEdGraphSchema_FroxEditor::TrySetDefaultObject(UEdGraphPin& InPin, UObject* NewDefaultObject) const
{
	Super::TrySetDefaultObject(InPin, NewDefaultObject);
}

void UEdGraphSchema_FroxEditor::TrySetDefaultText(UEdGraphPin& InPin, const FText& InNewDefaultText) const
{
	Super::TrySetDefaultText(InPin, InNewDefaultText);
}

bool UEdGraphSchema_FroxEditor::DoesDefaultValueMatchAutogenerated(const UEdGraphPin& InPin) const
{
	return Super::DoesDefaultValueMatchAutogenerated(InPin);
}

void UEdGraphSchema_FroxEditor::ResetPinToAutogeneratedDefaultValue(UEdGraphPin* Pin, bool bCallModifyCallbacks) const
{
	if (Pin->bOrphanedPin)
	{
		UEdGraphNode* Node = Pin->GetOwningNode();
		Node->PinConnectionListChanged(Pin);
	}
	else
	{
		// GetPinDefaultValuesFromString(Pin->PinType, Pin->GetOwningNodeUnchecked(), Pin->AutogeneratedDefaultValue, Pin->DefaultValue, Pin->DefaultObject, Pin->DefaultTextValue);

		if (bCallModifyCallbacks)
		{
			UEdGraphNode* Node = Pin->GetOwningNode();
			Node->PinDefaultValueChanged(Pin);
		}
	}
}
FLinearColor UEdGraphSchema_FroxEditor::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	const UGraphEditorSettings* Settings = GetDefault<UGraphEditorSettings>();

	if (PinType.PinCategory == UFroxNodeBase::PC_Frame)
	{
		return Settings->ExecutionPinTypeColor;
	}
	else if (PinType.PinCategory == UFroxNodeBase::PC_Property)
	{
		if (PinType.PinSubCategory == FFroxTypeDefinition::BoolType)
		{
			return Settings->BooleanPinTypeColor;
		}
		else if (
			PinType.PinSubCategory == FFroxTypeDefinition::IntType ||
			PinType.PinSubCategory == FFroxTypeDefinition::UInt8Type ||
			PinType.PinSubCategory == FFroxTypeDefinition::UInt16Type ||
			PinType.PinSubCategory == FFroxTypeDefinition::UInt32Type
		)
		{
			return Settings->IntPinTypeColor;
		}
		else if (PinType.PinSubCategory == FFroxTypeDefinition::EnumType)
		{
			return Settings->NamePinTypeColor;
		}
		else if (PinType.PinSubCategory == FFroxTypeDefinition::FloatType)
		{
			return Settings->FloatPinTypeColor;
		}
		else
		{
			return Settings->DefaultPinTypeColor;
		}
	}

	return Settings->WildcardPinTypeColor;
}

bool UEdGraphSchema_FroxEditor::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	check(Pin != NULL);

	if (Pin->bDefaultValueIsIgnored)
	{
		return true;
	}

	return false;
}

void UEdGraphSchema_FroxEditor::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Delete);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Cut);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Copy);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Paste);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().SelectAll);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Duplicate);
	if (InGraphPin)
	{
		MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
	}
	else
	{
		MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
	}

	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

void UEdGraphSchema_FroxEditor::SetPinAutogeneratedDefaultValue(UEdGraphPin* Pin, const FString& NewValue) const
{
	Pin->AutogeneratedDefaultValue = NewValue;

	ResetPinToAutogeneratedDefaultValue(Pin, false);
}