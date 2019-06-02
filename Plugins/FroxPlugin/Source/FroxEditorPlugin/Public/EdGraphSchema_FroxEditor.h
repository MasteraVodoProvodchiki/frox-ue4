#pragma once

#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_FroxEditor.generated.h"


/** Action to add a node to the graph */
USTRUCT()
struct FFroxSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	/** Template of node we want to create */
	UPROPERTY()
	class UEdGraphNode* NodeTemplate;


	FFroxSchemaAction_NewNode()
		: FEdGraphSchemaAction()
		, NodeTemplate(NULL)
	{}

	FFroxSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
		, NodeTemplate(NULL)
	{}

	// FEdGraphSchemaAction interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// End of FEdGraphSchemaAction interface

	template <typename NodeType>
	static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location, bool bSelectNewNode = true)
	{
		FFroxSchemaAction_NewNode Action;
		Action.NodeTemplate = InTemplateNode;

		return Cast<NodeType>(Action.PerformAction(ParentGraph, NULL, Location, bSelectNewNode));
	}
};

class FROXEDITORPLUGIN_API FroxSchemaUtils
{
public:
	template<typename T>
	static void AddAction(FString Title, FString Tooltip, TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, UEdGraph* OwnerOfTemporaries)
	{
		const FText MenuDesc = FText::FromString(Title);
		const FText Category = FText::FromString(TEXT("Nodes"));
		const FText TooltipText = FText::FromString(Tooltip);
		TSharedPtr<FFroxSchemaAction_NewNode> NewActorNodeAction = AddNewNodeAction(OutActions, Category, MenuDesc, TooltipText);
		T* ActorNode = NewObject<T>((UObject *)OwnerOfTemporaries);
		NewActorNodeAction->NodeTemplate = ActorNode;
	}

	static TSharedPtr<FFroxSchemaAction_NewNode> AddNewNodeAction(TArray<TSharedPtr<FEdGraphSchemaAction> >& OutActions, const FText& Category, const FText& MenuDesc, const FText& Tooltip)
	{
		TSharedPtr<FFroxSchemaAction_NewNode> NewAction = TSharedPtr<FFroxSchemaAction_NewNode>(new FFroxSchemaAction_NewNode(Category, MenuDesc, Tooltip, 0));
		OutActions.Add(NewAction);
		return NewAction;
	}
};


UCLASS()
class UEdGraphSchema_FroxEditor : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()

	// Begin EdGraphSchema interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	// End EdGraphSchema interface
};