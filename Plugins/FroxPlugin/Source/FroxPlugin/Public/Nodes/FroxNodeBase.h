#pragma once

#include "EdGraph/EdGraphNode.h"
#include "FroxNodeBase.generated.h"


template<typename TEnum>
static FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}

	return enumPtr->GetEnumName((int32)Value);
}

class FROXPLUGIN_API FNodePropertyObserver
{
public:
	virtual void OnPropertyChanged(class UEdGraphNode* Sender, const FName& PropertyName) = 0;
};

UCLASS()
class FROXPLUGIN_API UFroxNodeBase : public UEdGraphNode
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnNodeVisualsChanged, UFroxNodeBase*);

	static const FName PC_Frame;
	static const FName PC_Property;
	static const FName PC_Any;

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	bool CycleInspection(uint32 UniqueID);
#endif //WITH_EDITORONLY_DATA

	/** Identify that this node has undergone changes that will require synchronization with a compiled script.*/
	void MarkNodeRequiresSynchronization(FString Reason, bool bRaiseGraphNeedsRecompile);

	/** Get the change id for this node. This change id is updated whenever the node is manipulated in a way that should force a recompile.*/
	const FGuid& GetChangeId() const { return ChangeId; }

	FOnNodeVisualsChanged& OnVisualsChanged();

	TSharedPtr<FNodePropertyObserver> PropertyObserver;

protected:
	UPROPERTY()
	FGuid ChangeId;

	FOnNodeVisualsChanged VisualsChangedDelegate;
};
