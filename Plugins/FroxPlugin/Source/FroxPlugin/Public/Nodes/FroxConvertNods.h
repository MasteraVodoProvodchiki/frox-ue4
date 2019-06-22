#pragma once

#include "FroxArithmeticNods.h"
#include "FroxComputeFrame.h"
#include "FroxConvertNods.generated.h"

UCLASS()
class FROXPLUGIN_API UConvertToOpartionNode : public UOpartionInput1OuputNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "convertto"; }
	virtual const char* GetTitle() const override { return "ConvertTo"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;

public:
	/** type of frame  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EFroxTypeEnum Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Alpha = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Beta = 0.f;
};