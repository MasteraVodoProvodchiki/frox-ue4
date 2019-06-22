#pragma once

#include "FroxArithmeticNods.h"
#include "FroxCropNods.generated.h"

UCLASS()
class FROXPLUGIN_API UCropOpartionNode : public UOpartionInput1OuputNode
{
	GENERATED_BODY()

public:
	virtual const char* GetTypeName() const override { return "crop"; }
	virtual const char* GetTitle() const override { return "Crop"; }
	virtual frox::ComputeNode* CreateFroxNode(frox::ComputeFlow* Flow) const override;
};