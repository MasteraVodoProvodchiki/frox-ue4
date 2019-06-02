#pragma once

#include "Editor/UnrealEd/Public/Toolkits/AssetEditorToolkit.h"

#include "FroxComputeFlowAsset.h"
#include "GraphEditor.h"
#include "IDetailsView.h"
// #include "CustomNods.h"


class FFroxComputeFlowAssetEditor : public FAssetEditorToolkit, public FNotifyHook
{
public:
	~FFroxComputeFlowAssetEditor();

	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	void InitCustAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UFroxComputeFlowAsset* PropData);
	int N;

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

	void ShowMessage();


	TSharedRef<class SGraphEditor> CreateGraphEditorWidget(UEdGraph* InGraph);
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<FUICommandList> GraphEditorCommands;
	TSharedPtr<IDetailsView> PropertyEditor;
	UFroxComputeFlowAsset* PropBeingEdited;
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

	FDelegateHandle OnGraphChangedDelegateHandle;

	TSharedPtr<FExtender> ToolbarExtender;
	TSharedPtr<FUICommandList> MyToolBarCommands;
	bool bGraphStateChanged;
	void AddToolbarExtension(FToolBarBuilder &builder);
};