#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "EdGraph/EdGraph.h"
#include "Widgets/SWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "GraphEditor.h"
#include "FroxComputeFlowAssetEditor.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

/* Props View */
struct FFroxComputePropsViewSummoner : public FWorkflowTabFactory
{
public:
	FFroxComputePropsViewSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr);

	//~ Begin FWorkflowTabFactory
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	// End FWorkflowTabFactory

protected:
	TWeakPtr<class FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr;
};

/* Props Editor */
struct FFroxComputePropsEditorSummoner : public FWorkflowTabFactory
{
public:
	FFroxComputePropsEditorSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr);

	//~ Begin FWorkflowTabFactory
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	// End FWorkflowTabFactory

protected:
	TWeakPtr<class FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr;
};

/* Props Details */
struct FFroxComputePropsDetailsSummoner : public FWorkflowTabFactory
{
public:
	FFroxComputePropsDetailsSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr);

	//~ Begin FWorkflowTabFactory
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	// End FWorkflowTabFactory

protected:
	TWeakPtr<class FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr;
};

/* Flow Details */
struct FFroxComputeFlowDetailsSummoner : public FWorkflowTabFactory
{
public:
	FFroxComputeFlowDetailsSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr);

	//~ Begin FWorkflowTabFactory
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	// End FWorkflowTabFactory

protected:
	TWeakPtr<class FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr;
};

/* Flow Graph */
struct FFroxGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);
public:
	FFroxGraphEditorSummoner(TSharedPtr<class FFroxComputeFlowAssetEditor> InComputeFlowEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback);

	//~ Begin FDocumentTabFactory
	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;
	// End FDocumentTabFactory

protected:
	//~ Begin FDocumentTabFactoryForObjects
	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	// End FDocumentTabFactoryForObjects

	//~ Begin FDocumentTabFactory
	virtual void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const override;
	// End FDocumentTabFactory

protected:
	TWeakPtr<class FFroxComputeFlowAssetEditor> ComputeFlowEditorPtr;
	FOnCreateGraphEditorWidget OnCreateGraphEditorWidget;
};
