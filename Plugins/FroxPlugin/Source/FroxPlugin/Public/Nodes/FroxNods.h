#pragma once

#include "FroxNodeBase.h"
#include "FroxComputeFlowAsset.h"
#include "FroxNods.generated.h"

namespace frox {
	class ComputeNode;
	class ComputeFlow;
} // End frox

enum InputOutputPropertyNodeError : int
{
	IOPNE_None = 0,
	IOPNE_DuplicateKeys = 1
};

USTRUCT()
struct FComputeFlowNodeEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Frox)
	FName EntryName;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Frox, Meta = (ToolTip = "Optional description."))
	FString EntryDescription;

	UPROPERTY()
	FGuid UniqueId;
#endif // WITH_EDITORONLY_DATA

	/** key type and additional properties */
	UPROPERTY(EditAnywhere, Category = Frox)
	EComputeFlowKeyType KeyType;

	FComputeFlowNodeEntry()
		:
#if WITH_EDITORONLY_DATA
		UniqueId(FGuid::NewGuid()),
#endif
		KeyType(EComputeFlowKeyType::ECFKT_None)
	{}

	bool operator==(const FComputeFlowEntry& Other) const;
	bool operator==(const FComputeFlowNodeEntry& Other) const;
};

UCLASS()
class FROXPLUGIN_API UPropertyNode : public UFroxNodeBase
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ Begin UObject

	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName PropertyName;
};

UCLASS(Abstract)
class FROXPLUGIN_API UBaseNodeWithKeys : public UFroxNodeBase
{
	GENERATED_BODY()

public:
	UBaseNodeWithKeys(const FObjectInitializer& ObjectInitializer)
		: UFroxNodeBase(ObjectInitializer)
	{
		TestObject = 12345;
	}
	UPROPERTY(SimpleDisplay, Transient, EditDefaultsOnly)
	TArray<FComputeFlowNodeEntry> Keys;

	UPROPERTY(SimpleDisplay, Transient, EditDefaultsOnly)
	int TestObject;
};

UCLASS(HideDropDown)
class FROXPLUGIN_API UInputPropertyNode : public UBaseNodeWithKeys
{
	GENERATED_BODY()
		
public:
	UInputPropertyNode(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITORONLY_DATA
	//~ Begin UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent&) override;
	virtual void PostLoad() override;
	virtual void PreSave(const class ITargetPlatform*) override;
	//~ Begin UObject

	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode

	virtual bool CanUserDeleteNode() const override
	{
		return false;
	}

	virtual bool CanDuplicateNode() const override
	{
		return false;
	}
#endif //WITH_EDITORONLY_DATA
};

UCLASS(HideDropDown)
class FROXPLUGIN_API UOutputPropertyNode : public UBaseNodeWithKeys
{
	GENERATED_BODY()

public:
	UOutputPropertyNode(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITORONLY_DATA
	//~ Begin UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent&) override;

	virtual void PostLoad() override;
	virtual void PreSave(const class ITargetPlatform*) override;
	//~ Begin UObject

	virtual bool CanUserDeleteNode() const override
	{
		return false;
	}

	virtual bool CanDuplicateNode() const override
	{
		return false;
	}

	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA
};

UCLASS(Abstract)
class FROXPLUGIN_API UOpartionNode : public UFroxNodeBase
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const { return "none"; }
	virtual const char* GetTitle() const { return "none"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const;

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
#endif //WITH_EDITORONLY_DATA
};