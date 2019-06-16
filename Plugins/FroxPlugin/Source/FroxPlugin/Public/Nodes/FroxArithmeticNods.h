#pragma once

#include "FroxNods.h"
#include "FroxArithmeticNods.generated.h"

UCLASS(HideDropDown)
class FROXPLUGIN_API UOpartionBasicNode : public UOpartionNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const { return "none"; }

#if WITH_EDITORONLY_DATA
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
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
	virtual const char* GetTypeName() const override { return "add"; }
};

UCLASS()
class FROXPLUGIN_API USubOpartionNode : public UOpartionInput2OuputNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "sub"; }
};

UCLASS()
class FROXPLUGIN_API UMulOpartionNode : public UOpartionInput2OuputNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "mul"; }
};

UCLASS()
class FROXPLUGIN_API UDivOpartionNode : public UOpartionInput2OuputNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "div"; }
};

UCLASS()
class FROXPLUGIN_API UAvgOpartionNode : public UOpartionInput1OuputNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "avg"; }
};