// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FroxComputeFlowEditorTabFactories.h"
#include "Engine/Blueprint.h"
#include "EditorStyleSet.h"
#include "FroxEditorTabs.h"
#include "Widgets/Docking/SDockTab.h"
#include "FroxComputeFlowAsset.h"
#include "FroxComputeFlowAssetEditor.h"

#define LOCTEXT_NAMESPACE "FroxComputeFlowEditorFactories"

/////////////////////////////////////////////////////
// FFroxComputePropsViewSummoner
FFroxComputePropsViewSummoner::FFroxComputePropsViewSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr)
	: FWorkflowTabFactory(FFroxEditorTabs::PropsViewID, InComputeFlowEditorPtr)
	, ComputeFlowEditorPtr(InComputeFlowEditorPtr)
{
	TabLabel = LOCTEXT("FroxComputePropsLabel", "ComputeProps");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("FroxComputePropsView", "ComputeProps");
	ViewMenuTooltip = LOCTEXT("FroxComputePropsView_ToolTip", "Show the computeflow props view");
}

TSharedRef<SWidget> FFroxComputePropsViewSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(ComputeFlowEditorPtr.IsValid());
	return ComputeFlowEditorPtr.Pin()->SpawnComputePropsView();
}

FText FFroxComputePropsViewSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("FroxComputePropsTabTooltip", "The ComputeFlow props view is for viewing computeflow props key/value pairs.");
}

/////////////////////////////////////////////////////
// FFroxComputePropsEditorSummoner
FFroxComputePropsEditorSummoner::FFroxComputePropsEditorSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr)
	: FWorkflowTabFactory(FFroxEditorTabs::PropsEditorID, InComputeFlowEditorPtr)
	, ComputeFlowEditorPtr(InComputeFlowEditorPtr)
{
	TabLabel = LOCTEXT("FroxComputePropsLabel", "ComputeProps");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("FroxComputePropsEditor", "ComputeProps");
	ViewMenuTooltip = LOCTEXT("FroxComputePropsEditor_ToolTip", "Show the computeflow props editor");
}

TSharedRef<SWidget> FFroxComputePropsEditorSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(ComputeFlowEditorPtr.IsValid());
	return ComputeFlowEditorPtr.Pin()->SpawnComputePropsEditor();
}

FText FFroxComputePropsEditorSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("FroxComputePropsDetailsTabTooltip", "The Blackboard editor is for editing and debugging blackboard key/value pairs.");
}

/////////////////////////////////////////////////////
// FFroxComputePropsDetailsSummoner
FFroxComputePropsDetailsSummoner::FFroxComputePropsDetailsSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr)
	: FWorkflowTabFactory(FFroxEditorTabs::PropsDetailsID, InComputeFlowEditorPtr)
	, ComputeFlowEditorPtr(InComputeFlowEditorPtr)
{
	TabLabel = LOCTEXT("FroxComputePropsDetailsLabel", "ComuteProps Details");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("FroxComputePropsDetailsView", "Details");
	ViewMenuTooltip = LOCTEXT("FroxComputePropsDetailsView_ToolTip", "Show the details view");
}

TSharedRef<SWidget> FFroxComputePropsDetailsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(ComputeFlowEditorPtr.IsValid());
	return ComputeFlowEditorPtr.Pin()->SpawnComputePropsDetails();
}

FText FFroxComputePropsDetailsSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("FroxComputePropsDetailsTabTooltip", "The details tab is for editing computeflow entries.");
}

/////////////////////////////////////////////////////
// FFroxComputeFlowDetailsSummoner
FFroxComputeFlowDetailsSummoner::FFroxComputeFlowDetailsSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr)
	: FWorkflowTabFactory(FFroxEditorTabs::FlowDetailsID, InComputeFlowEditorPtr)
	, ComputeFlowEditorPtr(InComputeFlowEditorPtr)
{
	TabLabel = LOCTEXT("FroxComputeFlowDetailsLabel", "Details");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("FroxComputeFlowDetailsView", "Details");
	ViewMenuTooltip = LOCTEXT("FroxComputeFlowDetailsView_ToolTip", "Show the details view");
}

TSharedRef<SWidget> FFroxComputeFlowDetailsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(ComputeFlowEditorPtr.IsValid());
	return ComputeFlowEditorPtr.Pin()->SpawnProperties();
}

FText FFroxComputeFlowDetailsSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("FroxComputeFlowDetailsTabTooltip", "The compute flow details tab allows editing of the properties of compute flow nodes");
}

/////////////////////////////////////////////////////
// FFroxGraphEditorSummoner
FFroxGraphEditorSummoner::FFroxGraphEditorSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
	: FDocumentTabFactoryForObjects<UEdGraph>(FFroxEditorTabs::FlowEditorID, InComputeFlowEditorPtr)
	, ComputeFlowEditorPtr(InComputeFlowEditorPtr)
	, OnCreateGraphEditorWidget(CreateGraphEditorWidgetCallback)
{
}

void FFroxGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	check(ComputeFlowEditorPtr.IsValid());
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	// ComputeFlowEditorPtr.Pin()->OnGraphEditorFocused(GraphEditor);
}

void FFroxGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->NotifyGraphChanged();
}

TAttribute<FText> FFroxGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FFroxGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return OnCreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush* FFroxGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return FEditorStyle::GetBrush("NoBrush");
}

void FFroxGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	check(ComputeFlowEditorPtr.IsValid());
	check(ComputeFlowEditorPtr.Pin()->GetComputeFlow());

	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());

	FVector2D ViewLocation;
	float ZoomAmount;
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);

	UEdGraph* Graph = FTabPayload_UObject::CastChecked<UEdGraph>(Payload);
	// ComputeFlowEditorPtr.Pin()->GetComputeFlow()->LastEditedDocuments.Add(FEditedDocumentInfo(Graph, ViewLocation, ZoomAmount));
}

#undef LOCTEXT_NAMESPACE
