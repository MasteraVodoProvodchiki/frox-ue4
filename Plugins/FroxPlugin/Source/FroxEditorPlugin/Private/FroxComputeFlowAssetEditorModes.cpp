#include "FroxComputeFlowAssetEditorModes.h"

#include "FroxEditorTabs.h"
#include "FroxComputeFlowAssetEditor.h"
#include "FroxComputeFlowEditorTabFactories.h"
#include "FroxComputeFlowEditorToolbar.h"


/////////////////////////////////////////////////////
// FFroxComputeFlowEditorApplicationMode

#define LOCTEXT_NAMESPACE "FroxComputeFlowApplicationMode"

FFroxComputeFlowEditorApplicationMode::FFroxComputeFlowEditorApplicationMode(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditor)
	: FApplicationMode(FFroxComputeFlowAssetEditor::ComputeFlowMode, FFroxComputeFlowAssetEditor::GetLocalizedMode)
{
	ComputeFlowEditor = InComputeFlowEditor;

	ComputeFlowTabFactories.RegisterFactory(MakeShareable(new FFroxComputeFlowDetailsSummoner(InComputeFlowEditor)));
	ComputeFlowTabFactories.RegisterFactory(MakeShareable(new FFroxComputePropsViewSummoner(InComputeFlowEditor)));
	
	TabLayout = FTabManager::NewLayout("Standalone_FroxEditor_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(InComputeFlowEditor->GetToolbarTabId(), ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				// ->SetSizeCoefficient(0.2f)
				->Split
				(

					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->SetHideTabWell(true)
					->AddTab(FFroxEditorTabs::FlowEditorID, ETabState::ClosedTab)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.3f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.2f)
						->SetHideTabWell(true)
						->AddTab(FFroxEditorTabs::FlowDetailsID, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.8f)
						->AddTab(FFroxEditorTabs::PropsViewID, ETabState::OpenedTab)
					)
				)
			)
		);
	/*
	TabLayout = FTabManager::NewLayout("Standalone_FroxEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(InBehaviorTreeEditor->GetToolbarTabId(), ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(FBehaviorTreeEditorTabs::GraphEditorID, ETabState::ClosedTab)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.3f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.6f)
						->AddTab(FBehaviorTreeEditorTabs::GraphDetailsID, ETabState::OpenedTab)
						->AddTab(FBehaviorTreeEditorTabs::SearchID, ETabState::ClosedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.4f)
						->AddTab(FBehaviorTreeEditorTabs::BlackboardID, ETabState::OpenedTab)
					)
				)
			)
		);
	*/
	InComputeFlowEditor->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
	InComputeFlowEditor->GetToolbarBuilder()->AddComputeFlowToolbar(ToolbarExtender);
	
}

void FFroxComputeFlowEditorApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	check(ComputeFlowEditor.IsValid());
	TSharedPtr<FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr = ComputeFlowEditor.Pin();

	ComputeFlowEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());

	// Mode-specific setup
	ComputeFlowEditorPtr->PushTabFactories(ComputeFlowTabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FFroxComputeFlowEditorApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();

	check(ComputeFlowEditor.IsValid());
	TSharedPtr<FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr = ComputeFlowEditor.Pin();

	ComputeFlowEditorPtr->SaveEditedObjectState();
}

void FFroxComputeFlowEditorApplicationMode::PostActivateMode()
{
	// Reopen any documents that were open when the blueprint was last saved
	check(ComputeFlowEditor.IsValid());
	TSharedPtr<FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr = ComputeFlowEditor.Pin();
	ComputeFlowEditorPtr->RestoreComputeFlow();

	FApplicationMode::PostActivateMode();
}

#undef  LOCTEXT_NAMESPACE

/////////////////////////////////////////////////////
// FFroxComputePropsEditorApplicationMode

#define LOCTEXT_NAMESPACE "ComputePropsApplicationMode"

FFroxComputePropsEditorApplicationMode::FFroxComputePropsEditorApplicationMode(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditor)
	: FApplicationMode(FFroxComputeFlowAssetEditor::ComputePropsMode, FFroxComputeFlowAssetEditor::GetLocalizedMode)
{
	ComputeFlowEditor = InComputeFlowEditor;

/*	ComputePropsTabFactories.RegisterFactory(MakeShareable(new FFroxComputePropsEditorSummoner(InComputeFlowEditor)));
	ComputePropsTabFactories.RegisterFactory(MakeShareable(new FFroxComputePropsDetailsSummoner(InComputeFlowEditor)));
	*/

	TabLayout = FTabManager::NewLayout("Standalone_FroxPropsEditor_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(InComputeFlowEditor->GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				/*FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(FFroxEditorTabs::PropsEditorID, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(FFroxEditorTabs::PropsDetailsID, ETabState::OpenedTab)
				)*/
			)
		);
//*/
	
	InComputeFlowEditor->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
}

void FFroxComputePropsEditorApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	check(ComputeFlowEditor.IsValid());
	TSharedPtr<FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr = ComputeFlowEditor.Pin();

	ComputeFlowEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());

	// Mode-specific setup
	ComputeFlowEditorPtr->PushTabFactories(ComputePropsTabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FFroxComputePropsEditorApplicationMode::PostActivateMode()
{
	/*// Reopen any documents that were open when the BT was last saved
	check(ComputeFlowEditor.IsValid());
	TSharedPtr<FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr = ComputeFlowEditor.Pin();
	//	BehaviorTreeEditorPtr->StartEditingDefaults();*/

	FApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE
