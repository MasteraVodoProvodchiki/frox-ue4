#pragma once

#include "CoreMinimal.h"
#include "SlateFwd.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "FroxNods.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/SListView.h"

class TableRowBase;

class SFroxComputeFlowKeysEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFroxComputeFlowKeysEditor) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

	void SetObject(TWeakObjectPtr<UBaseNodeWithKeys> InNode);
private:
	friend class ItemFieldTableRow;
	friend class ItemHeaderTableRow;

	TSharedRef<SWidget> CreateKeyWidget(const FComputeFlowNodeEntry& InKey);
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<TableRowBase> ListItemPtr, const TSharedRef<STableViewBase>& OwnerTable);
	void OnSelectionChanged(TSharedPtr<TableRowBase> SelectedItem, ESelectInfo::Type SelectInfo);
	void RemoveItem(int InIndex);
	void ClearArray();
	void AddItem();
	void PreEditChange(UProperty* const InKeyStructProperty, int InArrayIndex = -1);
	void PostEditChange(UProperty* const InKeyStructProperty, int InArrayIndex = -1, EPropertyChangeType::Type InChangeType = EPropertyChangeType::Unspecified);

	void ConstructKey(int InIndex, UScriptStruct* InStructProperty, void* InStructValue);

	float OnGetLeftColumnWidth() const { return ColumnWidth; }
	float OnGetRightColumnWidth() const { return 1.0f - ColumnWidth; }
	void OnSetColumnWidth(float InWidth)
	{
		ColumnWidth = InWidth;
	}
	void OnSetRightColumnWidth(float) { /* Do nothing */ }


	TSharedPtr<SVerticalBox> DetailsLayout;
	TWeakObjectPtr<UBaseNodeWithKeys> Node;
	TSharedPtr<SListView<TSharedPtr<TableRowBase>>> ListView;
	TArray<TSharedPtr<TableRowBase>> Items;
	float ColumnWidth;
};
