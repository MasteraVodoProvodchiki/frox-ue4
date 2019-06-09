#include "FroxComputeFlowEditorToolbar.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Widgets/SWidget.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Widgets/Input/SComboButton.h"
#include "EditorStyleSet.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "FroxComputeFlowAssetEditor.h"
#include "WorkflowOrientedApp/SModeWidget.h"

// #include "BehaviorTreeEditorCommands.h"

#define LOCTEXT_NAMESPACE "FroxComputeFlowEditorToolbar"

class SFroxComputeFlowModeSeparator : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SFroxComputeFlowModeSeparator) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArg)
	{
		SBorder::Construct(
			SBorder::FArguments()
			.BorderImage(FEditorStyle::GetBrush("BlueprintEditor.PipelineSeparator"))
			.Padding(0.0f)
		);
	}

	// SWidget interface
	virtual FVector2D ComputeDesiredSize(float) const override
	{
		const float Height = 20.0f;
		const float Thickness = 16.0f;
		return FVector2D(Thickness, Height);
	}
	// End of SWidget interface
};

void FFroxComputeFlowEditorToolbar::AddModesToolbar(TSharedPtr<FExtender> Extender)
{
	check(ComputeFlowEditor.IsValid());
	TSharedPtr<FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr = ComputeFlowEditor.Pin();

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		ComputeFlowEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FFroxComputeFlowEditorToolbar::FillModesToolbar));
}

void FFroxComputeFlowEditorToolbar::AddComputeFlowToolbar(TSharedPtr<FExtender> Extender)
{
	check(ComputeFlowEditor.IsValid());
	TSharedPtr<FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr = ComputeFlowEditor.Pin();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		ComputeFlowEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, &FFroxComputeFlowEditorToolbar::FillComputeFlowToolbar));

	ComputeFlowEditorPtr->AddToolbarExtender(ToolbarExtender);
}

void FFroxComputeFlowEditorToolbar::FillModesToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(ComputeFlowEditor.IsValid());
	TSharedPtr<FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr = ComputeFlowEditor.Pin();

	TAttribute<FName> GetActiveMode(ComputeFlowEditorPtr.ToSharedRef(), &FFroxComputeFlowAssetEditor::GetCurrentMode);
	FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(ComputeFlowEditorPtr.ToSharedRef(), &FFroxComputeFlowAssetEditor::SetCurrentMode);

	// Left side padding
	ComputeFlowEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

	ComputeFlowEditorPtr->AddToolbarWidget(
		SNew(SModeWidget, FFroxComputeFlowAssetEditor::GetLocalizedMode(FFroxComputeFlowAssetEditor::ComputeFlowMode), FFroxComputeFlowAssetEditor::ComputeFlowMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		// .CanBeSelected(ComputeFlowEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CanAccessFlowMode)
		.ToolTipText(LOCTEXT("ComputeFlowModeButtonTooltip", "Switch to Compute Flow Mode"))
		.IconImage(FEditorStyle::GetBrush("FroxEditor.SwitchToComputeFlowMode"))
		.SmallIconImage(FEditorStyle::GetBrush("FroxEditor.SwitchToComputeFlowMode.Small"))
	);

	ComputeFlowEditorPtr->AddToolbarWidget(SNew(SFroxComputeFlowModeSeparator));

	ComputeFlowEditorPtr->AddToolbarWidget(
		SNew(SModeWidget, FFroxComputeFlowAssetEditor::GetLocalizedMode(FFroxComputeFlowAssetEditor::ComputePropsMode), FFroxComputeFlowAssetEditor::ComputePropsMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		// .CanBeSelected(ComputeFlowEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CanAccessPropsMode)
		.ToolTipText(LOCTEXT("ComputeFlowModeButtonTooltip", "Switch to Compute Props Mode"))
		.IconImage(FEditorStyle::GetBrush("FroxEditor.SwitchToComputeFlowMode"))
		.SmallIconImage(FEditorStyle::GetBrush("FroxEditor.SwitchToComputeFlowMode.Small"))
	);

	// Right side padding
	ComputeFlowEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));
}

void FFroxComputeFlowEditorToolbar::FillComputeFlowToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(ComputeFlowEditor.IsValid());
	TSharedPtr<FFroxComputeFlowAssetEditor> BehaviorTreeEditorPtr = ComputeFlowEditor.Pin();

	if (BehaviorTreeEditorPtr->GetCurrentMode() == FFroxComputeFlowAssetEditor::ComputeFlowMode)
	{
		ToolbarBuilder.BeginSection("ComputeProps");
		{
			// ToolbarBuilder.AddToolBarButton(FBTCommonCommands::Get().NewBlackboard);
		}
		ToolbarBuilder.EndSection();

		ToolbarBuilder.BeginSection("ComputeFlow");
		{
			const FText NewTaskLabel = LOCTEXT("NewTask_Label", "New Task");
			const FText NewTaskTooltip = LOCTEXT("NewTask_ToolTip", "Create a new task node Blueprint from a base class");
			const FSlateIcon NewTaskIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "BTEditor.Graph.NewTask");

			/*
			ToolbarBuilder.AddToolBarButton(
				FUIAction(
					FExecuteAction::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CreateNewTask),
					FCanExecuteAction::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CanCreateNewTask),
					FIsActionChecked(),
					FIsActionButtonVisible::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::IsNewTaskButtonVisible)
				),
				NAME_None,
				NewTaskLabel,
				NewTaskTooltip,
				NewTaskIcon
			);

			ToolbarBuilder.AddComboButton(
				FUIAction(
					FExecuteAction(),
					FCanExecuteAction::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CanCreateNewTask),
					FIsActionChecked(),
					FIsActionButtonVisible::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::IsNewTaskComboVisible)
				),
				FOnGetContent::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::HandleCreateNewTaskMenu),
				NewTaskLabel,
				NewTaskTooltip,
				NewTaskIcon
			);

			const FText NewDecoratorLabel = LOCTEXT("NewDecorator_Label", "New Decorator");
			const FText NewDecoratorTooltip = LOCTEXT("NewDecorator_ToolTip", "Create a new decorator node Blueprint from a base class");
			const FSlateIcon NewDecoratorIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "BTEditor.Graph.NewDecorator");

			ToolbarBuilder.AddToolBarButton(
				FUIAction(
					FExecuteAction::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CreateNewDecorator),
					FCanExecuteAction::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CanCreateNewDecorator),
					FIsActionChecked(),
					FIsActionButtonVisible::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::IsNewDecoratorButtonVisible)
				),
				NAME_None,
				NewDecoratorLabel,
				NewDecoratorTooltip,
				NewDecoratorIcon
			);

			ToolbarBuilder.AddComboButton(
				FUIAction(
					FExecuteAction(),
					FCanExecuteAction::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CanCreateNewDecorator),
					FIsActionChecked(),
					FIsActionButtonVisible::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::IsNewDecoratorComboVisible)
				),
				FOnGetContent::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::HandleCreateNewDecoratorMenu),
				NewDecoratorLabel,
				NewDecoratorTooltip,
				NewDecoratorIcon
			);

			const FText NewServiceLabel = LOCTEXT("NewService_Label", "New Service");
			const FText NewServiceTooltip = LOCTEXT("NewService_ToolTip", "Create a new service node Blueprint from a base class");
			const FSlateIcon NewServiceIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "BTEditor.Graph.NewService");

			ToolbarBuilder.AddToolBarButton(
				FUIAction(
					FExecuteAction::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CreateNewService),
					FCanExecuteAction::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CanCreateNewService),
					FIsActionChecked(),
					FIsActionButtonVisible::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::IsNewServiceButtonVisible)
				),
				NAME_None,
				NewServiceLabel,
				NewServiceTooltip,
				NewServiceIcon
			);

			ToolbarBuilder.AddComboButton(
				FUIAction(
					FExecuteAction(),
					FCanExecuteAction::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::CanCreateNewService),
					FIsActionChecked(),
					FIsActionButtonVisible::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::IsNewServiceComboVisible)
				),
				FOnGetContent::CreateSP(BehaviorTreeEditorPtr.Get(), &FFroxComputeFlowAssetEditor::HandleCreateNewServiceMenu),
				NewServiceLabel,
				NewServiceTooltip,
				NewServiceIcon
			);
			*/
		}
		ToolbarBuilder.EndSection();
	}
}

#undef LOCTEXT_NAMESPACE
