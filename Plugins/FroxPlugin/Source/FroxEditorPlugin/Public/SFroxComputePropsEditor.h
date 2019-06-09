#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SFroxComputePropsView.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeFlowEditor, Warning, All);

class FExtender;
class FMenuBuilder;
class FToolBarBuilder;
class FUICommandList;
class UFroxComputeFlowAsset;

/** Delegate used to determine whether the ComputeFlow mode is active */
DECLARE_DELEGATE_RetVal(bool, FOnIsComputeFlowModeActive);

/** Displays and edits ComputeProps entries */
class SFroxComputePropsEditor : public SFroxComputePropsView
{
public:
	SLATE_BEGIN_ARGS(SFroxComputePropsEditor) {}

	SLATE_EVENT(FOnEntrySelected, OnEntrySelected)
	SLATE_EVENT(FOnComputeFlowKeyChanged, OnComputeFlowKeyChanged)
	SLATE_EVENT(FOnIsComputeFlowModeActive, OnIsComputeFlowModeActive)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<FUICommandList> InCommandList, UFroxComputeFlowAsset* InComputeFlow);

private:
	/** Fill the context menu with edit options */
	virtual void FillContextMenu(FMenuBuilder& MenuBuilder) const override;

	/** Fill the toolbar with edit options */
	void FillToolbar(FToolBarBuilder& ToolbarBuilder) const;

	/** Extend the toolbar */
	virtual TSharedPtr<FExtender> GetToolbarExtender(TSharedRef<FUICommandList> ToolkitCommands) const override;

	/** Handle deleting the currently selected entry */
	void HandleDeleteEntry();

	/** Handle renaming the currently selected entry */
	void HandleRenameEntry();

	/** Create the menu used to create a new ComputeFlow entry */
	TSharedRef<SWidget> HandleCreateNewEntryMenu() const;

	/** Create a new ComputeFlow entry when a class is picked */
	void HandleKeyClassPicked(UClass* InClass);

	/** Delegate handler that disallows creating a new entry while the debugger is running */
	bool CanCreateNewEntry() const;

	/** Check whether the 'Delete' operation can be performed on the selected item  */
	bool CanDeleteEntry() const;

	/** Check whether the 'Rename' operation can be performed on the selected item  */
	bool CanRenameEntry() const;

private:
	/** Delegate used to determine whether the ComputeFlow mode is active */
	FOnIsComputeFlowModeActive OnIsComputeFlowModeActive;
};
