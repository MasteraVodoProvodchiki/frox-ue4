#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FFroxComputeFlowPropsCommands: public TCommands<FFroxComputeFlowPropsCommands>
{
public:
	FFroxComputeFlowPropsCommands();

	TSharedPtr<FUICommandInfo> DeleteEntry;

	/** Initialize commands */
	virtual void RegisterCommands() override;
};
