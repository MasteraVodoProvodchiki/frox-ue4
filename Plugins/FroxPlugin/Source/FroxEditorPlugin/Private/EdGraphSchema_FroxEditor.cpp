#include "EdGraphSchema_FroxEditor.h"

#include "FroxTypes.h"
#include "Nodes/FroxNods.h"
#include "Nodes/FroxArithmeticNods.h"
#include "Nodes/FroxMakeNods.h"
#include "Nodes/FroxConvertNods.h"
#include "Nodes/FroxCropNods.h"
#include "Nodes/FroxSubFlowNods.h"
#include "Nodes/FroxTaskNode.h"
#include "Nodes/FroxConstNods.h"
#include "Nodes/FroxFrameNods.h"
#include "Nodes/FroxFindContoursNode.h"
#include "Nodes/UE4ImageReaderNode.h"
#include "Nodes/UE4CustomNode.h"
#include "FroxSensorFrameNods.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "GraphEditorSettings.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "FroxConnectionDrawingPolicy.h"


#define SNAP_GRID (16) // @todo ensure this is the same as SNodePanel::GetSnapGridSize()


namespace
{
	// Maximum distance a drag can be off a node edge to require 'push off' from node
	const int32 NodeDistance = 60;
}

/// FFroxSchemaAction_NewNode
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

/// UEdGraphSchema_FroxEditor
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

	// FroxSchemaUtils::AddAction<UPropertyNode>(TEXT("Add Property"), TEXT("Add input property node to the prop graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	// FroxSchemaUtils::AddAction<UInputPropertyNode>(TEXT("Add Input Property"), TEXT("Add input property node to the graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);
	// FroxSchemaUtils::AddAction<UOutputPropertyNode>(TEXT("Add Output Property"), TEXT("Add output property node to the graph"), Actions, ContextMenuBuilder.OwnerOfTemporaries);

	UClass* Base = UFroxNodeBase::StaticClass();
	for (TObjectIterator< UClass > ClassIt; ClassIt; ++ClassIt)
	{
		UClass* Class = *ClassIt;

		// Only interested in native C++ classes
		if (!Class->IsNative())
		{
			continue;
		}

		// Ignore deprecated
		if (Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_Abstract | CLASS_HideDropDown))
		{
			continue;
		}

		// Check this class is a subclass of Base
		if (!Class->IsChildOf(Base))
		{
			continue;
		}

		// Add this class
		FString Title = FString::Format(TEXT("Add {0}"), TArray<FStringFormatArg>({ Class->GetDisplayNameText().ToString() }));
		FString ToolTip = FString::Format(TEXT("Add {0} to the graph"), TArray<FStringFormatArg>({ Class->GetDisplayNameText().ToString() }));
		TSharedPtr<FEdGraphSchemaAction> Action = FroxSchemaUtils::CreateAction<UFroxNodeBase>(Class, Title, ToolTip, ContextMenuBuilder.OwnerOfTemporaries);
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
	if (
		A->PinType.PinCategory != B->PinType.PinCategory &&
		A->PinType.PinCategory != UFroxNodeBase::PC_Any &&
		B->PinType.PinCategory != UFroxNodeBase::PC_Any
	)
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
	else if (PinType.PinCategory == UFroxNodeBase::PC_Data)
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
			PinType.PinSubCategory == FFroxTypeDefinition::UInt32Type ||
			PinType.PinSubCategory == FFroxTypeDefinition::PointType ||
			PinType.PinSubCategory == FFroxTypeDefinition::SizeType
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
		else if (PinType.PinSubCategory == FFroxTypeDefinition::Vector2DType)
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