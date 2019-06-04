#pragma once

#include "OperationLogic.generated.h"

UCLASS(Blueprintable, HideDropDown)
class UOperationLogic : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Logic")
	bool NodeInvoke(const FRandomStream& RandomStream);

	virtual const char* GetTypeName() const { return "none"; }
	virtual bool NodeInvoke_Implementation(const FRandomStream& RandomStream);
};

UCLASS(Blueprintable)
class UAddLogic : public UOperationLogic
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "add"; }
};

UCLASS(Blueprintable)
class USubLogic : public UOperationLogic
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "sub"; }
};

UCLASS(Blueprintable)
class UMulLogic : public UOperationLogic
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "mul"; }
};

UCLASS(Blueprintable)
class UDivLogic : public UOperationLogic
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "div"; }
};