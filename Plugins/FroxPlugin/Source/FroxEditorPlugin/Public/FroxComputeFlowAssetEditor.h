#pragma once

#include "Editor/UnrealEd/Public/Toolkits/AssetEditorToolkit.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

#include "Misc/NotifyHook.h"
#include "GraphEditor.h"
#include "IDetailsView.h"

class FDocumentTracker;
class FDocumentTabFactory;
class UFroxComputeFlowAsset;
struct FComputeFlowEntry;

class FFroxComputeFlowAssetEditor : public FWorkflowCentricApplication, public FNotifyHook
{
public:
	~FFroxComputeFlowAssetEditor();

	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	// virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	void InitCustAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UFroxComputeFlowAsset* PropData);

	//~ Begin IToolkit Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	//~ End IToolkit Interface

	/** Access the compute flow for this editor */
	UFroxComputeFlowAsset* GetComputeFlow() { return PropBeingEdited; }
	
	/** Access the toolbar builder for this editor */
	TSharedPtr<class FFroxComputeFlowEditorToolbar> GetToolbarBuilder() { return ToolbarBuilder; }

	// @todo This is a hack from BehaviorTreeEditor
	void RegisterToolbarTab(const TSharedRef<class FTabManager>& TabManager);

	/** Restores the compute flow we were editing or creates a new one if none is available */
	void RestoreComputeFlow();

	/** Save the graph state for later editing */
	void SaveEditedObjectState();

	/** Delegate handler for selection in the computeflow entry list */
	void HandleComputeFlowEntrySelected(const FComputeFlowEntry* ComputeFlowEntry, bool bIsInherited);

	/** Delegate handler for when a computeflow key changes (added, removed, renamed) */
	void HandleComputeFlowEntryChanged(UFroxComputeFlowAsset* InComputeFlow, FComputeFlowEntry* const InKey);

	/** Check whether computeflow mode is current */
	bool HandleIsComputeFlowPropsModeActive() const;

	TSharedRef<SWidget> SpawnProperties();
	TSharedRef<SWidget> SpawnComputePropsView();
	TSharedRef<SWidget> SpawnComputePropsEditor();
	TSharedRef<SWidget> SpawnComputePropsDetails();

	/**
	 * Get the localized text to display for the specified mode
	 * @param	InMode	The mode to display
	 * @return the localized text representation of the mode
	 */
	static FText GetLocalizedMode(FName InMode);

protected:
	void OnGraphChanged(const FEdGraphEditAction& Action);
	void SelectAllNodes();
	bool CanSelectAllNodes() const;
	void DeleteSelectedNodes();
	bool CanDeleteNode(class UEdGraphNode* Node);
	bool CanDeleteNodes() const;
	void DeleteNodes(const TArray<class UEdGraphNode*>& NodesToDelete);
	void CopySelectedNodes();
	bool CanCopyNodes() const;
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes() const;
	void CutSelectedNodes();
	bool CanCutNodes() const;
	void DuplicateNodes();
	bool CanDuplicateNodes() const;
	void DeleteSelectedDuplicatableNodes();

	/** Called when the selection changes in the GraphEditor */
	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);

	/** Called when a node is double clicked */
	void OnNodeDoubleClicked(class UEdGraphNode* Node);

	// void ShowMessage();


	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);

	FDelegateHandle OnGraphChangedDelegateHandle;


	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<FUICommandList> GraphEditorCommands;
	TSharedPtr<IDetailsView> PropertyEditor;
	UFroxComputeFlowAsset* PropBeingEdited;

	TSharedPtr<FDocumentTracker> DocumentManager;
	TWeakPtr<FDocumentTabFactory> GraphEditorTabFactoryPtr;

	TSharedPtr<class FFroxComputeFlowEditorToolbar> ToolbarBuilder;
	TSharedPtr<FExtender> ToolbarExtender;
	TSharedPtr<FUICommandList> MyToolBarCommands;

	/** The computeflow props view widget */
	TSharedPtr<class SFroxComputePropsView> ComputeFlowPropsView;

	/** The computeflow props editor widget */
	TSharedPtr<class SFroxComputePropsEditor> ComputeFlowPropsEditor;

	bool bGraphStateChanged;
	// void AddToolbarExtension(FToolBarBuilder &builder);

public:
	/** Modes in mode switcher */
	static const FName ComputeFlowMode;
	static const FName ComputePropsMode;
};