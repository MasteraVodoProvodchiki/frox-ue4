#pragma once

#include "Frox/Frox/ComputeData.h"

#include "FroxComputeData.generated.h"

UCLASS(BlueprintType, ClassGroup = (Frox))
class FROXPLUGIN_API UFroxComputeData : public UObject
{
	GENERATED_BODY()

public:
	void SetFroxData(frox::ComputeDataPtr FroxData);
	frox::ComputeDataPtr GetFroxData() const { return _froxData; }

private:
	frox::ComputeDataPtr _froxData;
};