#pragma once

#include "CoreMinimal.h"

class FFroxComputeFlowAssetEditor;
class FExtender;
class FToolBarBuilder;

class FFroxComputeFlowEditorToolbar : public TSharedFromThis<FFroxComputeFlowEditorToolbar>
{
public:
	FFroxComputeFlowEditorToolbar(TSharedPtr<FFroxComputeFlowAssetEditor> InComputeFlowEditor)
		: ComputeFlowEditor(InComputeFlowEditor) {}

	void AddModesToolbar(TSharedPtr<FExtender> Extender);
	void AddComputeFlowToolbar(TSharedPtr<FExtender> Extender);

private:
	void FillModesToolbar(FToolBarBuilder& ToolbarBuilder);
	void FillComputeFlowToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
	/** Pointer back to the blueprint editor tool that owns us */
	TWeakPtr<FFroxComputeFlowAssetEditor> ComputeFlowEditor;
};