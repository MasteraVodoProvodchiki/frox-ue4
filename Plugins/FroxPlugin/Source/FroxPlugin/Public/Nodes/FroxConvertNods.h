#pragma once

#include "FroxArithmeticNods.h"
#include "FroxComputeFrame.h"
#include "FroxConvertNods.generated.h"

UCLASS()
class FROXPLUGIN_API UConvertToOpartionNode : public UOpartionInput1OuputNode
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	//~ Begin UEdGraphNode
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode
#endif //WITH_EDITORONLY_DATA

	//~ Begin UOpartionNode
	virtual const char* GetTypeName() const override { return "convertto"; }
	virtual const char* GetTitle() const override { return "ConvertTo"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
	//~ End UOpartionNode

public:
	/** type of frame  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EFroxTypeEnum Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Alpha = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Beta = 0.f;
};