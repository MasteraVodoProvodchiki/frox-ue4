#pragma once

#include "FroxNods.h"
#include "FroxArithmeticNods.generated.h"

UCLASS(HideDropDown)
class FROXPLUGIN_API UOpartionBasicNode : public UOpartionNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UEdGraphNode

	//~ Begin UObject
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	//~ End UObject
#endif //WITH_EDITORONLY_DATA
};

UCLASS(HideDropDown)
class FROXPLUGIN_API UOpartionInput1OuputNode : public UOpartionBasicNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS(HideDropDown)
class FROXPLUGIN_API UOpartionInput2OuputNode : public UOpartionNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	virtual void AllocateDefaultPins() override;
#endif //WITH_EDITORONLY_DATA
};

UCLASS()
class FROXPLUGIN_API UAddOpartionNode : public UOpartionInput2OuputNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "add"; }
	virtual const char* GetTitle() const override { return "Add"; }
	//~ End UOpartionNode
};

UCLASS()
class FROXPLUGIN_API USubOpartionNode : public UOpartionInput2OuputNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "sub"; }
	virtual const char* GetTitle() const override { return "Subtract"; }
	//~ End UOpartionNode
};

UCLASS()
class FROXPLUGIN_API UMulOpartionNode : public UOpartionInput2OuputNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "mul"; }
	virtual const char* GetTitle() const override { return "Multiply"; }
	//~ End UOpartionNode
};

UCLASS()
class FROXPLUGIN_API UDivOpartionNode : public UOpartionInput2OuputNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "div"; }
	virtual const char* GetTitle() const override { return "Divide"; }
	//~ End UOpartionNode
};

UCLASS()
class FROXPLUGIN_API UAvgOpartionNode : public UOpartionInput1OuputNode
{
	GENERATED_BODY()

public:
	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "avg"; }
	virtual const char* GetTitle() const override { return "Mean"; }
	//~ End UOpartionNode
};