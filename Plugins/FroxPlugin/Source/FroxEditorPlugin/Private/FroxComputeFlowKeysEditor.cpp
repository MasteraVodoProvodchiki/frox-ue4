#include "FroxComputeFlowKeysEditor.h"

#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateBoxBrush.h"
#include "EditorStyleSet.h"
#include "EdGraph/EdGraph.h"
#include "Input/SComboBox.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateStyle.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"

namespace {

const int ITEM_HEIGHT = 26;
const TCHAR* KEYS_PROPERTY_NAME = TEXT("Keys");
const float LEFT_COLUMN_SIZE = 0.35f;
const int BASE_CELL_PADDING = 3;
const int LEVEL_PADDING = 15;
const int FIELD_WIDTH = 105;

FString GetPropertyDisplayName(const UProperty* Prop)
{
#if WITH_EDITOR
    static const FName DisplayNameKey(TEXT("DisplayName"));
    return (Prop && Prop->HasMetaData(DisplayNameKey)) ? Prop->GetMetaData(DisplayNameKey) : Prop->GetDisplayNameText().ToString();
#else  // WITH_EDITOR
    return Prop->GetName();
#endif // WITH_EDITOR
}

}

class TableRowBase
{
public:
    TableRowBase(SFroxComputeFlowKeysEditor *InEditor, int InIndex) : Index(InIndex), Editor(InEditor) {}

    virtual ~TableRowBase() {}

    virtual TSharedRef <ITableRow> OnGenerateRowForList(const TSharedRef <STableViewBase> &InOwnerTable) = 0;

    void SetLastCategoryRow(bool InLastRow) { LastCategoryRow = InLastRow; }

    virtual void SetIndex(int InIndex) { Index = InIndex; }

    int GetIndex() const { return Index; }

protected:
    bool LastCategoryRow = false;
    int Index;
    SFroxComputeFlowKeysEditor *Editor;
};

// ------------------------ ItemHeaderTableRow ------------------------------

class ItemHeaderTableRow : public TSharedFromThis<ItemHeaderTableRow>, public TableRowBase
{
public:
    ItemHeaderTableRow(SFroxComputeFlowKeysEditor *InEditor, int InIndex) : TableRowBase(InEditor, InIndex),
                                                                            Title(FString::FromInt(InIndex)),
                                                                            ArrayField(false) {}

    ItemHeaderTableRow(SFroxComputeFlowKeysEditor *InEditor, FString InTitle, bool InArrayField)
        : TableRowBase(InEditor, -1), Title(InTitle), ArrayField(InArrayField) {}

    TSharedRef <ITableRow> OnGenerateRowForList(const TSharedRef <STableViewBase> &InOwnerTable) override;

    FReply OnRemoveButtonClick();

    FReply OnAddButtonClick();

    FReply OnClearButtonClick();

    virtual void SetIndex(int InIndex)
    {
        TableRowBase::SetIndex(InIndex);
        Title = FString::FromInt(InIndex);
    }

    FText GetTitle() const { return FText::FromString(Title); }

private:
    FString Title;
    const bool ArrayField;
};

TSharedRef <ITableRow> ItemHeaderTableRow::OnGenerateRowForList(const TSharedRef <STableViewBase> &InOwnerTable)
{
    TSharedPtr <SWidget> FieldRowPanel;
    if (ArrayField)
    {
        SAssignNew(FieldRowPanel, SHorizontalBox)
        + SHorizontalBox::Slot()
        .VAlign(VAlign_Fill)
        [
            SNew(SButton)
            .ToolTipText(FText::FromString(TEXT("Add new key")))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Fill)
            .ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
            .TextStyle(FEditorStyle::Get(), "ContentBrowser.TopBar.Font")
            .ForegroundColor(FLinearColor::White)
            .OnClicked(this, &ItemHeaderTableRow::OnAddButtonClick)
            [
                SNew(SImage)
                .Image(FEditorStyle::GetBrush(TEXT("PropertyWindow.Button_AddToArray")))
            ]
        ]
        + SHorizontalBox::Slot()
        .VAlign(VAlign_Fill)
        [
            SNew(SButton)
            .ToolTipText(FText::FromString(TEXT("Add new key")))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Fill)
            .ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
            .TextStyle(FEditorStyle::Get(), "ContentBrowser.TopBar.Font")
            .ForegroundColor(FLinearColor::White)
            .OnClicked(this, &ItemHeaderTableRow::OnClearButtonClick)
            [
                SNew(SImage)
                .Image(FEditorStyle::GetBrush(TEXT("PropertyWindow.Button_EmptyArray")))
            ]
        ];
    }
    else
    {
        SAssignNew(FieldRowPanel, SButton)
        .ToolTipText(FText::FromString(TEXT("Remove the key")))
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Fill)
        .ButtonStyle(&FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
        .TextStyle(FEditorStyle::Get(), "ContentBrowser.TopBar.Font")
        .ForegroundColor(FLinearColor::White)
        .OnClicked(this, &ItemHeaderTableRow::OnRemoveButtonClick)
        [
            SNew(SImage)
            .Image(FEditorStyle::GetBrush(TEXT("PropertyWindow.Button_RemoveFromArray")))
        ];
    }
    return
        SNew(STableRow <TSharedPtr<TableRowBase>> , InOwnerTable)
        [
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush(
                    ArrayField ? "DetailsView.CategoryTop" : "DetailsView.CategoryMiddle"))
            .BorderBackgroundColor(ArrayField ? FLinearColor(.6, .6, .6, 1.0f) : FLinearColor())
            .Padding(0)
            [
                SNew(SBox)
                .HeightOverride(ITEM_HEIGHT)
                [
                    SNew(SSplitter)
                    .Style(FEditorStyle::Get(), "DetailsView.Splitter")
                    .PhysicalSplitterHandleSize(1.0f)
                    .HitDetectionSplitterHandleSize(5.0f)
                    + SSplitter::Slot()
                    .Value(TAttribute<float>(Editor, &SFroxComputeFlowKeysEditor::OnGetLeftColumnWidth))
                    .OnSlotResized(SSplitter::FOnSlotResized::CreateSP(Editor, &SFroxComputeFlowKeysEditor::OnSetColumnWidth))
                    [
                        SNew(SBox)
                        .Padding(FMargin(BASE_CELL_PADDING +
                                            (ArrayField ? 0 : LEVEL_PADDING),
                                            BASE_CELL_PADDING, BASE_CELL_PADDING,
                                            BASE_CELL_PADDING))
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                            .Font(FEditorStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
                            .ShadowOffset(FVector2D(1.0f, 1.0f))
                            .Text(this, &ItemHeaderTableRow::GetTitle)
                        ]
                    ]
                    + SSplitter::Slot()
                    .Value(TAttribute<float>(Editor, &SFroxComputeFlowKeysEditor::OnGetRightColumnWidth))
                    .OnSlotResized(SSplitter::FOnSlotResized::CreateSP(Editor, &SFroxComputeFlowKeysEditor::OnSetRightColumnWidth))
                    [
                        SNew(SBox)
                        .Padding(BASE_CELL_PADDING)
                        .HAlign(HAlign_Left)
                        .VAlign(VAlign_Fill)
                        [
                            FieldRowPanel.ToSharedRef()
                        ]
                    ]
                ]
            ]
        ];
}

FReply ItemHeaderTableRow::OnRemoveButtonClick()
{
    Editor->RemoveItem(Index);
    return FReply::Handled();
}

FReply ItemHeaderTableRow::OnAddButtonClick()
{
    Editor->AddItem();
    return FReply::Handled();
}

FReply ItemHeaderTableRow::OnClearButtonClick()
{
    Editor->ClearArray();
    return FReply::Handled();
}

// ------------------------ ItemFieldTableRow ------------------------------

class ItemFieldTableRow : public TSharedFromThis<ItemFieldTableRow>, public TableRowBase
{
public:
    ItemFieldTableRow(SFroxComputeFlowKeysEditor *InEditor, int InIndex, UProperty *const InProperty, void *InObject)
            : TableRowBase(InEditor, InIndex),
              Title(GetPropertyDisplayName(InProperty)),
              Property(InProperty),
              Object(InObject) {}

    TSharedRef <ITableRow> OnGenerateRowForList(const TSharedRef <STableViewBase> &InOwnerTable) override;

protected:
    TSharedPtr <SWidget> CreateValueWidget();

    TSharedPtr <SWidget> GetValueWidget() const { return ValueWidget; }

    FText GetTextValue() const;

    void OnTextCommitted(const FText &NewText, ETextCommit::Type /*CommitInfo*/);

    void GenerateComboBoxStrings();

    void OnComboSelectionChanged(TSharedPtr <FString> NewValue, ESelectInfo::Type SelectInfo);

    TSharedRef <SWidget> OnGenerateComboBoxWidget(TSharedPtr <FString> InComboString);

    bool IsReadOnly() const { return ReadOnly; }

    const FSlateBrush *GetBrush() const
    {
        return FEditorStyle::GetBrush(LastCategoryRow ? "DetailsView.CategoryBottom" : "DetailsView.CategoryMiddle");
    }

private:
    FString Title;
    UProperty *const Property;
    void *Object;
    bool ReadOnly = true;
    TSharedPtr <SWidget> ValueWidget;
    TArray <TSharedPtr<FString>> ComboBoxItems;
    TSharedPtr <FString> CurrentValue;
};

TSharedRef <ITableRow> ItemFieldTableRow::OnGenerateRowForList(const TSharedRef <STableViewBase>& InOwnerTable) {
    if (!ValueWidget)
        ValueWidget = CreateValueWidget();
    return
        SNew(STableRow<TSharedPtr<TableRowBase>>, InOwnerTable)
        .Style(FEditorStyle::Get(), "DetailsView.TreeView.TableRow")
        .ShowSelection(false)
        [
            SNew(SBorder)
            .BorderImage(this, &ItemFieldTableRow::GetBrush)
            .Padding(0)
            [
                SNew(SBox)
                .HeightOverride(ITEM_HEIGHT)
                .Padding(0)
                [
                    SNew(SSplitter)
                    .Style(FEditorStyle::Get(), "DetailsView.Splitter")
                    .PhysicalSplitterHandleSize(1.0f)
                    .HitDetectionSplitterHandleSize(5.0f)
                    +SSplitter::Slot()
                    .Value(TAttribute<float>(Editor, &SFroxComputeFlowKeysEditor::OnGetLeftColumnWidth))
                    .OnSlotResized(SSplitter::FOnSlotResized::CreateSP(Editor, &SFroxComputeFlowKeysEditor::OnSetColumnWidth))
                    [
                        SNew(SBox)
                        .Padding(FMargin(BASE_CELL_PADDING + 2 * LEVEL_PADDING,
                            BASE_CELL_PADDING, BASE_CELL_PADDING,
                            BASE_CELL_PADDING))
                        .HAlign(HAlign_Left)
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(Title))
                        ]
                    ]
                    +SSplitter::Slot()
                    .Value(TAttribute<float>(Editor, &SFroxComputeFlowKeysEditor::OnGetRightColumnWidth))
                    .OnSlotResized(SSplitter::FOnSlotResized::CreateSP(Editor, &SFroxComputeFlowKeysEditor::OnSetRightColumnWidth))
                    [
                        SNew(SBox)
                        .Padding(BASE_CELL_PADDING)
                        .HAlign(HAlign_Left)
                        [
                            SNew(SBox)
                            .HAlign(HAlign_Fill)
                            .WidthOverride(FIELD_WIDTH)
                            [
                                ValueWidget.ToSharedRef()
                            ]
                        ]
                    ]
                ]
            ]
        ];
}

TSharedPtr <SWidget> ItemFieldTableRow::CreateValueWidget()
{
    const UNameProperty *NameProperty = Cast<const UNameProperty>(Property);
    const UStrProperty *StrProperty = Cast<const UStrProperty>(Property);
    if (NameProperty || StrProperty)
    {
        CurrentValue = TSharedPtr<FString>(new FString(NameProperty
                                                       ? NameProperty->GetPropertyValue_InContainer(Object).ToString()
                                                       : StrProperty->GetPropertyValue_InContainer(Object)));
        ReadOnly = false;
        return
            SNew(SEditableTextBox)
            .Text(this, &ItemFieldTableRow::GetTextValue)
            .SelectAllTextWhenFocused(true)
            .ClearKeyboardFocusOnCommit(false)
            .OnTextCommitted(this, &ItemFieldTableRow::OnTextCommitted)
            .SelectAllTextOnCommit(true)
            .IsReadOnly(this, &ItemFieldTableRow::IsReadOnly)
            .IsPassword(false);
    }
    const UByteProperty *ByteProperty = Cast<const UByteProperty>(Property);
    const UEnumProperty *EnumProperty = Cast<const UEnumProperty>(Property);
    if (ByteProperty && ByteProperty->Enum || EnumProperty)
    {
        ReadOnly = false;
        GenerateComboBoxStrings();
        TSharedPtr <STextBlock> Block;
        return
            SNew(SComboBox <TSharedPtr<FString>> )
            [
                SNew(STextBlock)
                    .Text(this, &ItemFieldTableRow::GetTextValue)
            ]
            .OptionsSource(&ComboBoxItems)
            .InitiallySelectedItem(CurrentValue)
            .OnGenerateWidget(this, &ItemFieldTableRow::OnGenerateComboBoxWidget)
            .OnSelectionChanged(this, &ItemFieldTableRow::OnComboSelectionChanged);
    }
    CurrentValue = TSharedPtr<FString>(new FString(FString("Unsupported ") + Property->GetClass()->GetName()));
    return
        SNew(STextBlock)
        .Text(this, &ItemFieldTableRow::GetTextValue);
}

FText ItemFieldTableRow::GetTextValue() const
{
    return FText::FromString(*CurrentValue);
}

void ItemFieldTableRow::OnTextCommitted(const FText &NewText, ETextCommit::Type /*CommitInfo*/)
{
    if (ReadOnly || NewText.ToString() == *CurrentValue)
        return;

    CurrentValue = TSharedPtr<FString>(new FString(NewText.ToString()));
    Editor->PreEditChange(Property, Index);
    if (const UNameProperty *NameProperty = Cast<const UNameProperty>(Property))
    {
        NameProperty->SetPropertyValue_InContainer(Object, FName(*NewText.ToString()));
    }
    else if (const UStrProperty *StrProperty = Cast<const UStrProperty>(Property))
    {
        StrProperty->SetPropertyValue_InContainer(Object, NewText.ToString());
    }
    Editor->PostEditChange(Property, Index, EPropertyChangeType::ValueSet);
}

void ItemFieldTableRow::GenerateComboBoxStrings()
{
    UEnum *Enum = nullptr;
    int64 Value = -1;
    if (const UEnumProperty *EnumProperty = Cast<const UEnumProperty>(Property))
    {
        Enum = EnumProperty->GetEnum();
        Value = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(
                EnumProperty->ContainerPtrToValuePtr<void>(Object));
    }
    else
    {
        const UByteProperty *ByteProperty = CastChecked<const UByteProperty>(Property);
        Enum = ByteProperty->Enum;
        Value = ByteProperty->GetPropertyValue_InContainer(Object);
    }
    ComboBoxItems.Empty();
    CurrentValue = nullptr;
    for (int32 EnumIdx = 0; EnumIdx < Enum->NumEnums() - 1; ++EnumIdx)
    {
        bool bShouldBeHidden = Enum->HasMetaData(TEXT("Hidden"), EnumIdx) || Enum->HasMetaData(TEXT("Spacer"), EnumIdx);
        if (!bShouldBeHidden)
        {
            auto ValuePtr = TSharedPtr<FString>(new FString(Enum->GetDisplayNameTextByIndex(EnumIdx).ToString()));
            ComboBoxItems.Add(ValuePtr);
            if (Enum->GetValueByIndex(EnumIdx) == Value)
            {
                CurrentValue = ValuePtr;
            }
        }
    }
}

TSharedRef <SWidget> ItemFieldTableRow::OnGenerateComboBoxWidget(TSharedPtr <FString> InComboBoxString)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*InComboBoxString));
}

void ItemFieldTableRow::OnComboSelectionChanged(TSharedPtr <FString> NewValue, ESelectInfo::Type SelectInfo)
{
    if (ReadOnly || NewValue == CurrentValue)
        return;
    CurrentValue = NewValue;
    const UEnum *Enum = nullptr;
    const UNumericProperty *TargetProperty = nullptr;
    void *Data = nullptr;
    if (const UEnumProperty *EnumProperty = Cast<const UEnumProperty>(Property))
    {
        Enum = EnumProperty->GetEnum();
        TargetProperty = EnumProperty->GetUnderlyingProperty();
        Data = EnumProperty->ContainerPtrToValuePtr<void>(Object);
    }
    else
    {
        const UByteProperty *ByteProperty = CastChecked<const UByteProperty>(Property);
        Enum = ByteProperty->Enum;
        TargetProperty = ByteProperty;
        Data = ByteProperty->ContainerPtrToValuePtr<void>(Object);
    }
    for (int32 EnumIdx = 0; EnumIdx < Enum->NumEnums() - 1; ++EnumIdx)
    {
        if (*NewValue == Enum->GetDisplayNameTextByIndex(EnumIdx).ToString())
        {
            Editor->PreEditChange(Property, Index);
            TargetProperty->SetIntPropertyValue(Data, Enum->GetValueByIndex(EnumIdx));
            Editor->PostEditChange(Property, Index, EPropertyChangeType::ValueSet);
            break;
        }
    }
}

// ------------------------ SFroxComputeFlowKeysEditor ------------------------------

void SFroxComputeFlowKeysEditor::Construct(const FArguments& InArgs)
{
	ColumnWidth = LEFT_COLUMN_SIZE;
	//FButtonStyle AddButtonStyle = FButtonStyle(FEditorStyle::Get().GetWidgetStyle<FButtonStyle>("FlatButton.DarkGrey"))
		//.SetNormal(FSlateColorBrush(FLinearColor(FColor(20, 20, 20))));
	ChildSlot
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(ListView, SListView<TSharedPtr<TableRowBase>>)
				//.SelectionMode(ESelectionMode::Type::None)
				.ItemHeight(ITEM_HEIGHT)
				.ListItemsSource(&Items)
				.OnGenerateRow(this, &SFroxComputeFlowKeysEditor::OnGenerateRowForList)
			]
	    ];
}

void SFroxComputeFlowKeysEditor::SetObject(TWeakObjectPtr<UBaseNodeWithKeys> InNode)
{
	Node = InNode;
	Items.Reset();
	static const FName KeysPropertyName = KEYS_PROPERTY_NAME;
	for (TFieldIterator<UProperty> PropertyIterator(Node->GetClass()); PropertyIterator; ++PropertyIterator)
	{
		UProperty* Property = *PropertyIterator;
		FName const PropertyName = Property->GetFName();
		if (PropertyName == KeysPropertyName && Property->IsA<UArrayProperty>())
		{
            auto* ArrayProperty = CastChecked<UArrayProperty>(Property);
            auto* InnerProperty = CastChecked<UStructProperty>(ArrayProperty->Inner);
			Items.Add(TSharedPtr<ItemHeaderTableRow>(new ItemHeaderTableRow(this, GetPropertyDisplayName(Property), true)));
            auto& Keys = Node->Keys;
			for (int I = 0; I < Keys.Num(); I++)
            {
                ConstructKey(I, InnerProperty->Struct, &Keys[I]);
			}
			break;
		}
	}
	ListView->RequestListRefresh();
}

void SFroxComputeFlowKeysEditor::ConstructKey(int InIndex, UScriptStruct* InStructProperty, void* InStructValue)
{
	if (Items.Num() > 0) Items.Last(0)->SetLastCategoryRow(false);

	Items.Add(TSharedPtr<ItemHeaderTableRow>(new ItemHeaderTableRow(this, InIndex)));
	for (TFieldIterator<UProperty> PropertyIterator(InStructProperty); PropertyIterator; ++PropertyIterator)
    {
		UProperty* Property = *PropertyIterator;
		if (!Property->HasAnyPropertyFlags(EPropertyFlags::CPF_Edit))
        {
			continue;
		}
		auto DisplayName = GetPropertyDisplayName(Property);
		const void* ValuePtr = Property->ContainerPtrToValuePtr<void>(InStructValue);
		Items.Add(TSharedPtr<ItemFieldTableRow>(new ItemFieldTableRow(this, InIndex, Property, InStructValue)));
	}
	Items.Last(0)->SetLastCategoryRow(true);
}

TSharedRef<ITableRow> SFroxComputeFlowKeysEditor::OnGenerateRowForList(TSharedPtr<TableRowBase> ListItemPtr, const TSharedRef<STableViewBase>& OwnerTable)
{
	return ListItemPtr->OnGenerateRowForList(OwnerTable);
}

void SFroxComputeFlowKeysEditor::OnSelectionChanged(TSharedPtr<TableRowBase> InSelectedItem, ESelectInfo::Type InSelectInfo)
{
	
}

void SFroxComputeFlowKeysEditor::RemoveItem(int InIndex)
{
	if (InIndex < 0)
        return;

	PreEditChange(nullptr, InIndex);
	Node->Keys.RemoveAt(InIndex);
	PostEditChange(nullptr, InIndex, EPropertyChangeType::ArrayRemove);
	if (Items.Num() > 0) Items.Last(0)->SetLastCategoryRow(false);
	for (int I = Items.Num() - 1; I >= 0; I--)
    {
		if (Items[I]->GetIndex() == InIndex)
		{
			Items.RemoveAt(I);
		}
		else if (Items[I]->GetIndex() > InIndex)
		{
			Items[I]->SetIndex(Items[I]->GetIndex() - 1);
		}
		else
		{
			break;
		}
	}
	if (Items.Num() > 0)
        Items.Last(0)->SetLastCategoryRow(true);

	ListView->RequestListRefresh();
}

void SFroxComputeFlowKeysEditor::ClearArray()
{
	PreEditChange(nullptr, -1);
	Node->Keys.Reset();
	PostEditChange(nullptr, -1, EPropertyChangeType::ArrayClear);

	for (int i = Items.Num() - 1; i >= 0; i--)
	{
		if (Items[i]->GetIndex() >= 0)
			Items.RemoveAt(i);
	}
	ListView->RequestListRefresh();
}

void SFroxComputeFlowKeysEditor::AddItem()
{
    PreEditChange(nullptr, -1);
    Node->Keys.Add(FComputeFlowNodeEntry());
    PostEditChange(nullptr, -1, EPropertyChangeType::ArrayAdd);

	ConstructKey(Node->Keys.Num() - 1, FComputeFlowNodeEntry::StaticStruct(), &Node->Keys.Last(0));
	ListView->RequestListRefresh();
}

void SFroxComputeFlowKeysEditor::PreEditChange(UProperty* InKeyStructProperty, int InArrayIndex)
{
	auto* Property = Node->GetClass()->FindPropertyByName(KEYS_PROPERTY_NAME);
	FEditPropertyChain Chain;
	Chain.AddHead(Property);
	if (InKeyStructProperty)
    {
		Chain.AddTail(InKeyStructProperty);
	}
	Node->PreEditChange(Chain);
}

void SFroxComputeFlowKeysEditor::PostEditChange(UProperty* InKeyStructProperty, int InArrayIndex, EPropertyChangeType::Type InChangeType)
{
	// TODO(guilder13): I have no idea why and what happens here.
	// First of all why event must be chain, even though simple event can handle the same type of changes.
	auto* Property = Node->GetClass()->FindPropertyByName(KEYS_PROPERTY_NAME);
	FEditPropertyChain Chain;
	Chain.AddHead(Property);
	if (InKeyStructProperty)
    {
		Chain.AddTail(InKeyStructProperty);
	}

	TArray<TMap<FString, int32>> Array;
	if (InArrayIndex >= 0)
    {
		TMap<FString, int32> Map;
		Map.Add(KEYS_PROPERTY_NAME) = InArrayIndex;
		Array.Add(Map);
	}
	FPropertyChangedEvent PostEditEvent(Property, InChangeType);
	PostEditEvent.ObjectIteratorIndex = 0;
	PostEditEvent.SetArrayIndexPerObject(Array);
	FPropertyChangedChainEvent ChainEvent(Chain, PostEditEvent);
	Node->PostEditChangeChainProperty(ChainEvent);
}
