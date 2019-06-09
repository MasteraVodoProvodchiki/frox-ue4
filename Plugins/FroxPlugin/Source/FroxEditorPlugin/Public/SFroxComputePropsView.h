#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "EdGraph/EdGraphSchema.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputePropsView, Warning, All);

class FExtender;
class FMenuBuilder;
class FUICommandList;
class SGraphActionMenu;
class UFroxComputeFlowAsset;
struct FComputeFlowEntry;
struct FCreateWidgetForActionData;

/** Delegate executed when an entry is selected */
DECLARE_DELEGATE_TwoParams(FOnEntrySelected, const FComputeFlowEntry*, bool);

/** Delegate for when a ComputeFlow key changes (added, removed, renamed) */
DECLARE_DELEGATE_TwoParams(FOnComputeFlowKeyChanged, UFroxComputeFlowAsset* /*InComputeFlow*/, FComputeFlowEntry* const /*InKey*/);

/** Compute flow entry in the list */
class FEdGraphSchemaAction_ComputeFlowEntry : public FEdGraphSchemaAction_Dummy
{
public:
	static FName StaticGetTypeId();
	virtual FName GetTypeId() const;

	FEdGraphSchemaAction_ComputeFlowEntry(UFroxComputeFlowAsset* InComputeFlow, FComputeFlowEntry& InKey, bool bInIsInherited);

	void Update();

	/** ComputeFlow we reference our key in */
	UFroxComputeFlowAsset* ComputeFlow;

	/** Actual key */
	FComputeFlowEntry& Key;

	/** Whether this entry came from a parent ComputeFlow */
	// bool bIsInherited;

	/** Temp flag for new items */
	bool bIsNew;
};

/** Displays computeprops entries */
class SFroxComputePropsView : public SCompoundWidget, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SFroxComputePropsView)
	{
		_IsReadOnly = true;
	}

	SLATE_EVENT(FOnEntrySelected, OnEntrySelected)
	SLATE_EVENT(FOnComputeFlowKeyChanged, OnComputeFlowKeyChanged)
	SLATE_ARGUMENT(bool, IsReadOnly)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<FUICommandList> InCommandList, UFroxComputeFlowAsset* InComputeFlow);

	/** FGCObject implementation */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	FComputeFlowEntry* GetSelectedEntry(bool& bOutIsInherited) const;
	int32 GetSelectedEntryIndex(bool& bOutIsInherited) const;

	/** Set the object we are looking at */
	void SetObject(UFroxComputeFlowAsset* InComputeFlow);

protected:
	/** Delegate handler used to generate a widget for an 'action' (key) in the list */
	TSharedRef<SWidget> HandleCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData);

	/** Delegate handler used to collect all 'actions' (keys) for display */
	void HandleCollectAllActions(FGraphActionListBuilderBase& GraphActionListBuilder);

	/** Get the title of the specified section ID */
	FText HandleGetSectionTitle(int32 SectionID) const;

	/** Delegate handler used when an action is selected */
	void HandleActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& SelectedActions, ESelectInfo::Type InSelectionType) const;

	/** Delegate handler used to generate an action's context menu */
	TSharedPtr<SWidget> HandleContextMenuOpening(TSharedRef<FUICommandList> ToolkitCommands) const;

	/** Add any context menu items */
	virtual void FillContextMenu(FMenuBuilder& MenuBuilder) const;

	/** Extend the toolbar */
	virtual TSharedPtr<FExtender> GetToolbarExtender(TSharedRef<FUICommandList> ToolkitCommands) const;

	/** Check whether we have any items selected */
	bool HasSelectedItems() const;

	/** Helper function for GetSelectedEntry() */
	TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> GetSelectedEntryInternal() const;

	/** Delegate handler used to match an FName to an action in the list, used for renaming keys */
	bool HandleActionMatchesName(FEdGraphSchemaAction* InAction, const FName& InName) const;

protected:
	/** The ComputeFlow we are editing/viewing */
	UFroxComputeFlowAsset* ComputeFlow;

	/** The list of ComputeFlow entries */
	TSharedPtr<SGraphActionMenu> GraphActionMenu;

	/** Delegate executed when an entry is selected */
	FOnEntrySelected OnEntrySelected;

	/** Delegate for when a compute props key changes (added, removed, renamed) */
	FOnComputeFlowKeyChanged OnComputeFlowKeyChanged;
};
