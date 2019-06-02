#pragma once

#include "OperationLogic.generated.h"

UCLASS(Blueprintable, HideDropDown)
class UOperationLogic : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Logic")
	bool NodeInvoke(const FRandomStream& RandomStream);

	virtual bool NodeInvoke_Implementation(const FRandomStream& RandomStream);
};

UCLASS(Blueprintable, HideDropDown)
class UAddLogic : public UOperationLogic
{
	GENERATED_BODY()

public:
};

UCLASS(Blueprintable, HideDropDown)
class USubLogic : public UOperationLogic
{
	GENERATED_BODY()

public:
};