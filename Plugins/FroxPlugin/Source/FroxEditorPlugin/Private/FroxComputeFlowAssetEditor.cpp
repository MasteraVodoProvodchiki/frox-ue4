#include "FroxComputeFlowAssetEditor.h"

#include "FroxComputeFlowAsset.h"
#include "FroxComputeFlowAssetEditorModes.h"
#include "FroxComputeFlowEditorToolbar.h"
#include "FroxComputeFlowEditorTabFactories.h"
#include "FroxComputeFlowGraph.h"
#include "EdGraphSchema_FroxEditor.h"
#include "SFroxComputePropsEditor.h"
#include "FroxComputeFlowCommands.h"
#include "FroxEditorPlugin.h"
#include "FroxComputePropsDataDetails.h"

#include "SDockTab.h"
#include "GenericCommands.h"
#include "BlueprintEditorUtils.h"
#include "EdGraphUtilities.h"
#include "ScopedTransaction.h"
#include "SNodePanel.h"
#include "ToolBarCommands.h"
#include "BlueprintEditorModule.h"
#include "PropertyEditorModule.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

#define LOCTEXT_NAMESPACE "FroxComputeFlowAssetEditor"


const FName FFroxComputeFlowAssetEditor::ComputeFlowMode(TEXT("ComputeFlow"));
const FName FFroxComputeFlowAssetEditor::ComputePropsMode(TEXT("ComputeProps"));

FFroxComputeFlowAssetEditor::~FFroxComputeFlowAssetEditor()
{}

FName FFroxComputeFlowAssetEditor::GetToolkitFName() const
{
	return FName("ComputeFlow Editor");
}

FText FFroxComputeFlowAssetEditor::GetBaseToolkitName() const
{
	return FText::FromString("ComputeFlow EditorToolkitName");
}

FLinearColor FFroxComputeFlowAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FFroxComputeFlowAssetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "ComputeFlow ").ToString();
}

FText FFroxComputeFlowAssetEditor::GetToolkitName() const
{
	return LOCTEXT("AppLabel", "ComputeFlow");
}

FText FFroxComputeFlowAssetEditor::GetToolkitToolTipText() const
{
	return LOCTEXT("AppLabel", "ComputeFlow");
}



void FFroxComputeFlowAssetEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		if (ComputeFlowPropsView.IsValid())
		{
			ComputeFlowPropsView->SetObject(GetComputeFlow());
		}

		if (ComputeFlowPropsEditor.IsValid())
		{
			ComputeFlowPropsEditor->SetObject(GetComputeFlow());
		}
	}
}

void FFroxComputeFlowAssetEditor::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
		if (ComputeFlowPropsView.IsValid())
		{
			ComputeFlowPropsView->SetObject(GetComputeFlow());
		}

		if (ComputeFlowPropsEditor.IsValid())
		{
			ComputeFlowPropsEditor->SetObject(GetComputeFlow());
		}
	}
}

void FFroxComputeFlowAssetEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{
	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		if (ComputeFlowPropsView.IsValid())
		{
			ComputeFlowPropsView->SetObject(GetComputeFlow());
		}
		if (ComputeFlowPropsEditor.IsValid())
		{
			ComputeFlowPropsEditor->SetObject(GetComputeFlow());
		}
	}
}

void FFroxComputeFlowAssetEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);

	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FFroxComputeFlowAssetEditor::InitCustAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UFroxComputeFlowAsset* PropData)
{
	FAssetEditorManager::Get().CloseOtherEditors(PropData, this);
	PropBeingEdited = PropData;

	TSharedPtr<FFroxComputeFlowAssetEditor> ThisPtr(SharedThis(this));
	if (!DocumentManager.IsValid())
	{
		DocumentManager = MakeShareable(new FDocumentTracker);
		DocumentManager->Initialize(ThisPtr);

		// Register the document factories
		{
			TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FFroxGraphEditorSummoner(ThisPtr,
				FFroxGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FFroxComputeFlowAssetEditor::CreateGraphEditorWidget)
			));

			// Also store off a reference to the grapheditor factory so we can find all the tabs spawned by it later.
			GraphEditorTabFactoryPtr = GraphEditorFactory;
			DocumentManager->RegisterDocumentFactory(GraphEditorFactory);
		}
	}

	// Toolbar
	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FFroxComputeFlowEditorToolbar(SharedThis(this)));
	}

	// Register commands
	FFroxComputeFlowPropsCommands::Register();

	// Initialize the asset editor and spawn nothing (dummy layout)
	const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
	
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	InitAssetEditor(Mode, InitToolkitHost, FFroxEditorPluginModule::FroxEditorAppIdentifier, DummyLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, PropData);

	// BindCommonCommands();

	AddApplicationMode(ComputeFlowMode, MakeShareable(new FFroxComputeFlowEditorApplicationMode(SharedThis(this))));
	AddApplicationMode(ComputePropsMode, MakeShareable(new FFroxComputePropsEditorApplicationMode(SharedThis(this))));

	// Enteries View
	ComputeFlowPropsView = SNew(SFroxComputePropsView, GetToolkitCommands(), GetComputeFlow());

	// Enteries Editor
	ComputeFlowPropsEditor = SNew(SFroxComputePropsEditor, GetToolkitCommands(), GetComputeFlow())
		.OnEntrySelected(this, &FFroxComputeFlowAssetEditor::HandleComputeFlowEntrySelected)
		.OnComputeFlowKeyChanged(this, &FFroxComputeFlowAssetEditor::HandleComputeFlowEntryChanged)
		.OnIsComputeFlowModeActive(this, &FFroxComputeFlowAssetEditor::HandleIsComputeFlowPropsModeActive);

	SetCurrentMode(ComputeFlowMode);

	// RegenerateMenusAndToolbars();
}

void FFroxComputeFlowAssetEditor::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FFroxComputeFlowAssetEditor::RestoreComputeFlow()
{
	UFroxComputeFlowGraph* FroxGraph = Cast<UFroxComputeFlowGraph>(PropBeingEdited->UpdateGraph);
	const bool bNewGraph = FroxGraph == NULL;
	if (!PropBeingEdited->UpdateGraph)
	{
		PropBeingEdited->UpdateGraph = FBlueprintEditorUtils::CreateNewGraph(
			PropBeingEdited,
			TEXT("Compute Flow"),
			UFroxComputeFlowGraph::StaticClass(),
			UEdGraphSchema_FroxEditor::StaticClass()
		);
		FroxGraph = Cast<UFroxComputeFlowGraph>(PropBeingEdited->UpdateGraph);

		// Initialize the behavior tree graph
		const UEdGraphSchema* Schema = FroxGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*FroxGraph);

		// OnCreated
	}
	else
	{
		// OnLoaded
	}

	// FroxGraph->Initialize();

	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(FroxGraph);
	TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, bNewGraph ? FDocumentTracker::OpenNewDocument : FDocumentTracker::RestorePreviousDocument);

	/*
	if (PropBeingEdited->LastEditedDocuments.Num() > 0)
	{
		TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(DocumentTab->GetContent());
		GraphEditor->SetViewLocation(PropBeingEdited->LastEditedDocuments[0].SavedViewOffset, PropBeingEdited->LastEditedDocuments[0].SavedZoomAmount);
	}
	*/

	if (bNewGraph)
	{
		//
	}
	else
	{
		//
	}

	// Update
}

void FFroxComputeFlowAssetEditor::SaveEditedObjectState()
{
	// Clear currently edited documents
	// PropBeingEdited->LastEditedDocuments.Empty();

	// Ask all open documents to save their state, which will update LastEditedDocuments
	DocumentManager->SaveAllState();
}


void FFroxComputeFlowAssetEditor::HandleComputeFlowEntrySelected(const FComputeFlowEntry* ComputeFlowEntry, bool bIsInherited)
{
	// refresh details view
	const bool bForceRefresh = true;

	if (ensure(ComputePropsDetailsView.IsValid()))
	{
		ComputePropsDetailsView->SetObject(GetComputeFlow(), bForceRefresh);
	}
}

void FFroxComputeFlowAssetEditor::HandleComputeFlowEntryChanged(UFroxComputeFlowAsset* InComputeFlow, FComputeFlowEntry* const InKey)
{
	if (ComputeFlowPropsView.IsValid())
	{
		// re-set object in computeflow view to keep it up to date
		ComputeFlowPropsView->SetObject(InComputeFlow);
	}
}

bool FFroxComputeFlowAssetEditor::HandleIsComputeFlowPropsModeActive() const
{
	return GetCurrentMode() == ComputePropsMode;
}

int32 FFroxComputeFlowAssetEditor::HandleGetSelectedBlackboardItemIndex(bool& bIsInherited)
{
	if (ComputeFlowPropsEditor.IsValid())
	{
		return ComputeFlowPropsEditor->GetSelectedEntryIndex(bIsInherited);
	}

	return INDEX_NONE;
}

TSharedRef<SWidget> FFroxComputeFlowAssetEditor::SpawnProperties()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	//DetailsViewArgs.NotifyHook = this;
	//DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;

	TSharedRef<IDetailsView> PropertyEditorRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	PropertyEditor = PropertyEditorRef;

	//DetailsView->SetObject(NULL);
	//DetailsView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateSP(this, &FFroxComputeFlowAssetEditor::IsPropertyEditable));
	//DetailsView->OnFinishedChangingProperties().AddSP(this, &FFroxComputeFlowAssetEditor::OnFinishedChangingProperties);

	return PropertyEditorRef;

	/*
	// Spawn the tab
	return SNew(SDockTab)
		.Label(FText::FromString("Details"))
		[
			PropertyEditorRef
		];
	*/
}

TSharedRef<SWidget> FFroxComputeFlowAssetEditor::SpawnComputePropsView()
{
	return ComputeFlowPropsView.ToSharedRef();
}

TSharedRef<SWidget> FFroxComputeFlowAssetEditor::SpawnComputePropsEditor()
{
	return ComputeFlowPropsEditor.ToSharedRef();
}

TSharedRef<SWidget> FFroxComputeFlowAssetEditor::SpawnComputePropsDetails()
{
	const bool bIsUpdatable = false;
	const bool bIsLockable = false;
	const bool bAllowSearch = true;
	const bool bHideSelectionTip = true;

	// Create DetailsView
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(bIsUpdatable, bIsLockable, bAllowSearch, FDetailsViewArgs::HideNameArea, bHideSelectionTip);
	DetailsViewArgs.NotifyHook = this;
	ComputePropsDetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	// Connect select index
	FOnGetSelectedComputePropsItemIndex OnGetSelectedBlackboardItemIndex = FOnGetSelectedComputePropsItemIndex::CreateSP(this, &FFroxComputeFlowAssetEditor::HandleGetSelectedBlackboardItemIndex);
	FOnGetDetailCustomizationInstance LayoutVariableDetails = FOnGetDetailCustomizationInstance::CreateStatic(&FFroxComputePropsDataDetails::MakeInstance, OnGetSelectedBlackboardItemIndex);
	ComputePropsDetailsView->RegisterInstancedCustomPropertyLayout(UFroxComputeFlowAsset::StaticClass(), LayoutVariableDetails);

	// Set Object
	UFroxComputeFlowAsset* ComputeFlow = GetComputeFlow();
	ComputePropsDetailsView->SetObject(ComputeFlow);

	// Return
	return ComputePropsDetailsView.ToSharedRef();
}

TSharedRef<SGraphEditor> FFroxComputeFlowAssetEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
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

	// Make title bar
	TSharedRef<SWidget> TitleBarWidget = 
		SNew(SBorder)
		.BorderImage( FEditorStyle::GetBrush( TEXT("Graph.TitleBackground") ) )
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("FroxComputeFlowGraphLabel", "ComputeFlow Tree"))
				.TextStyle( FEditorStyle::Get(), TEXT("GraphBreadcrumbButtonText") )
			]
		];

	// Create the appearance info
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = FText::FromString("Compute Flow Editor");


	// Make full graph editor
	TSharedRef<SGraphEditor> _GraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.TitleBar(TitleBarWidget)
		.Appearance(AppearanceInfo)
		.GraphToEdit(InGraph)
		.GraphEvents(InEvents);

	// Del GraphEditor
	GraphEditor = _GraphEditor;
	return _GraphEditor;
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

FText FFroxComputeFlowAssetEditor::GetLocalizedMode(FName InMode)
{
	static TMap< FName, FText > LocModes;

	if (LocModes.Num() == 0)
	{
		LocModes.Add(ComputeFlowMode, LOCTEXT("ComputeFlowMode", "Compute Flow"));
		LocModes.Add(ComputePropsMode, LOCTEXT("ComputePropsMode", "Compute Props"));
	}

	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);
	return *OutDesc;
}

#undef LOCTEXT_NAMESPACE