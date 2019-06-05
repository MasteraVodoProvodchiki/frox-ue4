#pragma once

#include "EdGraph/EdGraphNode.h"
#include "OperationLogic.h"
#include "FroxNods.generated.h"

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

/*
UENUM(BlueprintType)
enum class ERootType
{
	Type1,
	Type2
};
*/

struct FPinDataTypes
{
	static const FString PinType_Root;
};

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
	virtual TArray<UFroxNodeBase*> GetChildNodes(FRandomStream& RandomStream);

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	bool CycleInspection(uint32 UniqueID);
#endif //WITH_EDITORONLY_DATA

	TSharedPtr<FNodePropertyObserver> PropertyObserver;	
};

UCLASS()
class FROXPLUGIN_API UInputPropertyNode : public UFroxNodeBase
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
#endif //WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName PropertyName;
};

UCLASS()
class FROXPLUGIN_API UOutputPropertyNode : public UFroxNodeBase
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
#endif //WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName PropertyName;
};

UCLASS(HideDropDown)
class FROXPLUGIN_API UOpartionNode : public UFroxNodeBase
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const { return "none"; }

#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
#endif //WITH_EDITORONLY_DATA

	virtual TArray<UFroxNodeBase*> GetChildNodes(FRandomStream& RandomStream) override;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	// TSubclassOf<UOperationLogic> Operation;

	// UPROPERTY()
	//UOperationLogic* OperationInstance;
};

UCLASS()
class FROXPLUGIN_API UAddOpartionNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "add"; }
};

UCLASS()
class FROXPLUGIN_API USubOpartionNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "sub"; }
};

UCLASS()
class FROXPLUGIN_API UMulOpartionNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "mul"; }
};

UCLASS()
class FROXPLUGIN_API UDivOpartionNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "div"; }
};