#include "FroxComputeFlowAssetEditor.h"

// #include "UICustomEditor.h"
#include "EdGraphSchema_FroxEditor.h"
#include "SDockTab.h"
#include "GenericCommands.h"
#include "BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "ScopedTransaction.h"
#include "SNodePanel.h"
#include "ToolBarCommands.h"
#include "BlueprintEditorModule.h"
#include "PropertyEditorModule.h"
// #include "CustomNods.h"
// #include "MyStructCustomization.h"


const FName CustomEditorAppName = FName(TEXT("CustomEditorApp"));

FFroxComputeFlowAssetEditor::~FFroxComputeFlowAssetEditor()
{
	if (GraphEditor->GetCurrentGraph())
	{
		GraphEditor->GetCurrentGraph()->RemoveOnGraphChangedHandler(OnGraphChangedDelegateHandle);
	}
}

struct FFroxEditorTabs
{
	static const FName DetailsID;
	static const FName ViewportID;
};

const FName FFroxEditorTabs::DetailsID(TEXT("Details"));
const FName FFroxEditorTabs::ViewportID(TEXT("Viewport"));


FName FFroxComputeFlowAssetEditor::GetToolkitFName() const
{
	return FName("UICustom Editor");
}

FText FFroxComputeFlowAssetEditor::GetBaseToolkitName() const
{
	return FText::FromString("UICustom EditorToolkitName");
}

FLinearColor FFroxComputeFlowAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FFroxComputeFlowAssetEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("CustomEditor");
}


void FFroxComputeFlowAssetEditor::InitCustAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UFroxComputeFlowAsset* PropData)
{
	FAssetEditorManager::Get().CloseOtherEditors(PropData, this);
	PropBeingEdited = PropData;

	if (!PropBeingEdited->UpdateGraph)
	{
		UEdGraph* flowGraph = NewObject<UEdGraph>(PropBeingEdited, UEdGraph::StaticClass(), NAME_None, RF_Transactional);
		flowGraph->Schema = UEdGraphSchema_FroxEditor::StaticClass();
		PropBeingEdited->UpdateGraph = flowGraph;
	}

	GraphEditor = CreateGraphEditorWidget(PropBeingEdited->UpdateGraph);



	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("FroxEditor_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.2f)
				->Split
				(

					FTabManager::NewStack()
					->SetSizeCoefficient(0.8f)
					->SetHideTabWell(true)
					->AddTab(FFroxEditorTabs::ViewportID, ETabState::OpenedTab)

				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.2f)
					->SetHideTabWell(true)
					->AddTab(FFroxEditorTabs::DetailsID, ETabState::OpenedTab)
				)


			)

		);

	//
	MyToolBarCommands = MakeShareable(new FUICommandList);
	MyToolBarCommands->MapAction(FToolBarCommandsCommands::Get().ShowMessage,
		FExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::ShowMessage)
	);

	ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, MyToolBarCommands, FToolBarExtensionDelegate::CreateRaw(this, &FFroxComputeFlowAssetEditor::AddToolbarExtension));
	AddToolbarExtender(ToolbarExtender);

	// Initialize the asset editor and spawn nothing (dummy layout)
	InitAssetEditor(Mode, InitToolkitHost, CustomEditorAppName, StandaloneDefaultLayout, /*bCreateDefaultStandaloneMenu=*/ true, /*bCreateDefaultToolbar=*/ true, PropData);

	// Listen for graph changed event
	OnGraphChangedDelegateHandle = GraphEditor->GetCurrentGraph()->AddOnGraphChangedHandler(FOnGraphChanged::FDelegate::CreateRaw(this, &FFroxComputeFlowAssetEditor::OnGraphChanged));
	bGraphStateChanged = true;
}

void FFroxComputeFlowAssetEditor::AddToolbarExtension(FToolBarBuilder &builder)
{
	FSlateIcon IconBrush = FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.ViewOptions", "LevelEditor.ViewOptions.Small");

	builder.AddToolBarButton(FToolBarCommandsCommands::Get().ShowMessage, NAME_None, FText::FromString("Show Message"), FText::FromString("Click to show message"), IconBrush, NAME_None);
}

void FFroxComputeFlowAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	WorkspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(FText::FromString("Custom Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(TabManager);

	TabManager->RegisterTabSpawner(FFroxEditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FFroxComputeFlowAssetEditor::SpawnTab_Viewport))
		.SetDisplayName(FText::FromString("Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	TabManager->RegisterTabSpawner(FFroxEditorTabs::DetailsID, FOnSpawnTab::CreateSP(this, &FFroxComputeFlowAssetEditor::SpawnTab_Details))
		.SetDisplayName(FText::FromString("Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FFroxComputeFlowAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(TabManager);

	TabManager->UnregisterTabSpawner(FFroxEditorTabs::ViewportID);
	TabManager->UnregisterTabSpawner(FFroxEditorTabs::DetailsID);

}

TSharedRef<SDockTab> FFroxComputeFlowAssetEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.Label(FText::FromString("Mesh Graph"))
		.TabColorScale(GetTabColorScale())
		[
			GraphEditor.ToSharedRef()
		];

}

TSharedRef<SDockTab> FFroxComputeFlowAssetEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	TSharedRef<IDetailsView> PropertyEditorRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PropertyEditor = PropertyEditorRef;

	// Spawn the tab
	return SNew(SDockTab)
		.Label(FText::FromString("Details"))
		[
			PropertyEditorRef
		];
}


TSharedRef<SGraphEditor> FFroxComputeFlowAssetEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	// Create the appearance info
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString("Mesh tree Editor");

	GraphEditorCommands = MakeShareable(new FUICommandList);
	{
		GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
			FExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::SelectAllNodes),
			FCanExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::CanSelectAllNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
			FExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::DeleteSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::CanDeleteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
			FExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::CopySelectedNodes),
			FCanExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::CanCopyNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
			FExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::PasteNodes),
			FCanExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::CanPasteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
			FExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::CutSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::CanCutNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
			FExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::DuplicateNodes),
			FCanExecuteAction::CreateSP(this, &FFroxComputeFlowAssetEditor::CanDuplicateNodes)
		);


	}

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FFroxComputeFlowAssetEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FFroxComputeFlowAssetEditor::OnNodeDoubleClicked);

	if (InGraph->Schema)
	{
		//UE_LOG(UICustomEditor, Warning, TEXT("Not Schem"));
	}
	else
	{
		//UE_LOG(UICustomEditor, Warning, TEXT("Schem"))
	}

	TSharedRef<SGraphEditor> _GraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.Appearance(AppearanceInfo)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents)
		;
	return _GraphEditor;
}


void FFroxComputeFlowAssetEditor::ShowMessage()
{
	// FMyClassDetails::ShowNotification("Message Show");
}

// Editor Actions......................................................................................................

void FFroxComputeFlowAssetEditor::OnSelectedNodesChanged(const TSet<UObject*>& NewSelection)
{
	TArray<UObject*> SelectedObjects;
	for (UObject* Object : NewSelection)
	{
		SelectedObjects.Add(Object);
	}
	PropertyEditor->SetObjects(SelectedObjects);
}

void FFroxComputeFlowAssetEditor::OnNodeDoubleClicked(UEdGraphNode* Node)
{

}

void FFroxComputeFlowAssetEditor::SelectAllNodes()
{
	GraphEditor->SelectAllNodes();
}

bool FFroxComputeFlowAssetEditor::CanSelectAllNodes() const
{
	return GraphEditor.IsValid();
}

void FFroxComputeFlowAssetEditor::DeleteSelectedNodes()
{
	TArray<UEdGraphNode*> NodesToDelete;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		NodesToDelete.Add(CastChecked<UEdGraphNode>(*NodeIt));
	}

	DeleteNodes(NodesToDelete);
}

bool FFroxComputeFlowAssetEditor::CanDeleteNode(UEdGraphNode* Node)
{
	return true;
}

bool FFroxComputeFlowAssetEditor::CanDeleteNodes() const
{
	return true;
}

void FFroxComputeFlowAssetEditor::DeleteNodes(const TArray<UEdGraphNode*>& NodesToDelete)
{
	if (NodesToDelete.Num() > 0)
	{

		for (int32 Index = 0; Index < NodesToDelete.Num(); ++Index)
		{
			if (!CanDeleteNode(NodesToDelete[Index])) {
				continue;
			}

			// Break all node links first so that we don't update the material before deleting
			NodesToDelete[Index]->BreakAllNodeLinks();

			FBlueprintEditorUtils::RemoveNode(NULL, NodesToDelete[Index], true);

			// TODO: Process deletion in the data model
		}
	}
}

void FFroxComputeFlowAssetEditor::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			Node->PrepareForCopying();
		}
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
	FPlatformMisc::ClipboardCopy(*ExportedText);

	// Make sure the owner remains the same for the copied node
	// TODO: Check MaterialEditor.cpp for reference

}

bool FFroxComputeFlowAssetEditor::CanCopyNodes() const
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if ((Node != NULL) && Node->CanDuplicateNode())
		{
			return true;
		}
	}
	return false;
}

void FFroxComputeFlowAssetEditor::PasteNodes()
{
	PasteNodesHere(GraphEditor->GetPasteLocation());
}

void FFroxComputeFlowAssetEditor::PasteNodesHere(const FVector2D& Location)
{
	// Undo/Redo support
	const FScopedTransaction Transaction(FText::FromString("Dungeon Editor: Paste"));
	// TODO: Notify the data model of modification
	//Material->MaterialGraph->Modify();
	//Material->Modify();

	// Clear the selection set (newly pasted stuff will be selected)
	GraphEditor->ClearSelectionSet();

	// Grab the text to paste from the clipboard.
	FString TextToImport;
	FPlatformMisc::ClipboardPaste(TextToImport);

	// Import the nodes
	if (!PropBeingEdited) return;
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(PropBeingEdited->UpdateGraph, TextToImport, /*out*/ PastedNodes);

	//Average position of nodes so we can move them while still maintaining relative distances to each other
	FVector2D AvgNodePosition(0.0f, 0.0f);

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;
		AvgNodePosition.X += Node->NodePosX;
		AvgNodePosition.Y += Node->NodePosY;
	}

	if (PastedNodes.Num() > 0)
	{
		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;


		// Select the newly pasted stuff
		GraphEditor->SetNodeSelection(Node, true);

		Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
		Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		// Give new node a different Guid from the old one
		Node->CreateNewGuid();
	}

	// TODO: Implement
	//UpdatePropAfterGraphChange();

	// Update UI
	GraphEditor->NotifyGraphChanged();
}

bool FFroxComputeFlowAssetEditor::CanPasteNodes() const
{
	FString ClipboardContent;
	FPlatformMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(PropBeingEdited->UpdateGraph, ClipboardContent);
}

void FFroxComputeFlowAssetEditor::CutSelectedNodes()
{
	CopySelectedNodes();
	// Cut should only delete nodes that can be duplicated
	DeleteSelectedDuplicatableNodes();
}

bool FFroxComputeFlowAssetEditor::CanCutNodes() const
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FFroxComputeFlowAssetEditor::DuplicateNodes()
{
	// Copy and paste current selection
	CopySelectedNodes();
	PasteNodes();
}

bool FFroxComputeFlowAssetEditor::CanDuplicateNodes() const
{
	return CanCopyNodes();
}

void FFroxComputeFlowAssetEditor::DeleteSelectedDuplicatableNodes()
{
	// Cache off the old selection
	const FGraphPanelSelectionSet OldSelectedNodes = GraphEditor->GetSelectedNodes();

	// Clear the selection and only select the nodes that can be duplicated
	FGraphPanelSelectionSet RemainingNodes;
	GraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if ((Node != NULL) && Node->CanDuplicateNode())
		{
			GraphEditor->SetNodeSelection(Node, true);
		}
		else
		{
			RemainingNodes.Add(Node);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	// Reselect whatever is left from the original selection after the deletion
	GraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(RemainingNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			GraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FFroxComputeFlowAssetEditor::OnGraphChanged(const FEdGraphEditAction& Action)
{
	bGraphStateChanged = true;
}