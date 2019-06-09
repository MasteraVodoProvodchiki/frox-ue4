#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/ApplicationMode.h"

/** Main */
class FFroxComputeFlowEditorApplicationMode : public FApplicationMode
{
public:
	FFroxComputeFlowEditorApplicationMode(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditor);

	//~ Begin FApplicationMode
	virtual void RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	//~ End FApplicationMode

protected:
	TWeakPtr<class FFroxComputeFlowAssetEditor> ComputeFlowEditor;

	// Set of spawnable tabs in behavior tree editing mode
	FWorkflowAllowedTabSet ComputeFlowTabFactories;
};

/** Props */
class FFroxComputePropsEditorApplicationMode : public FApplicationMode
{
public:
	FFroxComputePropsEditorApplicationMode(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditor);

	//~ Begin FApplicationMode
	virtual void RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager) override;
	virtual void PostActivateMode() override;
	//~ End FApplicationMode

protected:
	TWeakPtr<class FFroxComputeFlowAssetEditor> ComputeFlowEditor;

	// Set of spawnable tabs in blackboard mode
	FWorkflowAllowedTabSet ComputePropsTabFactories;
};
