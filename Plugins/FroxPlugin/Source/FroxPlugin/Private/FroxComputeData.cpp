#include "FroxComputeData.h"
#include "Shared.h"
#include "FroxPlugin.h"

#include "Frox/Frox/Frox.h"

void UFroxComputeData::SetFroxData(frox::ComputeDataPtr FroxData)
{
	check(FroxData != nullptr);
	_froxData = FroxData;
}