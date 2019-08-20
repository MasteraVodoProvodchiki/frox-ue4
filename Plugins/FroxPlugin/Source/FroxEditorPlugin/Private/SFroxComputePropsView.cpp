// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SFroxComputePropsView.h"

#include "FroxComputeFlowAsset.h"

#include "SGraphPalette.h"
#include "EditorStyleSet.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateIconFinder.h"
#include "Styling/CoreStyle.h"
#include "ScopedTransaction.h"
#include "AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "SFroxComputePropsView"

DEFINE_LOG_CATEGORY(LogComputePropsView);

namespace EComputePropsSectionTitles
{
	enum Type
	{
		InheritedKeys = 1,
		Keys,
	};
}

FName FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId()
{
	static FName Type("FEdGraphSchemaAction_ComputeFlowEntry"); return Type;
}

FName FEdGraphSchemaAction_ComputeFlowEntry::GetTypeId() const
{
	return StaticGetTypeId();
}

FEdGraphSchemaAction_ComputeFlowEntry::FEdGraphSchemaAction_ComputeFlowEntry(UFroxComputeFlowAsset* InComputeFlow, FComputeFlowEntry& InKey, bool bInIsInherited)
	: FEdGraphSchemaAction_Dummy()
	, ComputeFlow(InComputeFlow)
	, Key(InKey)
	//, bIsInherited(bInIsInherited)
	, bIsNew(false)
{
	check(ComputeFlow);
	Update();
}

template<typename TEnum>
static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}
	return enumPtr->GetNameByValue((int64)Value).ToString();
}

void FEdGraphSchemaAction_ComputeFlowEntry::Update()
{
	UpdateSearchData(FText::FromName(Key.EntryName), FText::Format(LOCTEXT("ComputePropsEntryFormat", "{0} '{1}'"), FText::FromString(GetEnumValueAsString("EComputeFlowKeyType", Key.KeyType)), FText::FromName(Key.EntryName)), FText(), FText());
	SectionID = EComputePropsSectionTitles::Keys; // bIsInherited ? EComputePropsSectionTitles::InheritedKeys : EComputePropsSectionTitles::Keys;
}

class SFroxComputeFlowPropsItem : public SGraphPaletteItem
{
	SLATE_BEGIN_ARGS(SFroxComputeFlowPropsItem) {}

	SLATE_EVENT(FOnComputeFlowKeyChanged, OnComputeFlowKeyChanged)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData)
	{
		OnComputeFlowKeyChanged = InArgs._OnComputeFlowKeyChanged;

		const FSlateFontInfo NameFont = FCoreStyle::GetDefaultFontStyle("Regular", 10);

		check(InCreateData);
		check(InCreateData->Action.IsValid());

		TSharedPtr<FEdGraphSchemaAction> GraphAction = InCreateData->Action;
		check(GraphAction->GetTypeId() == FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId());
		TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> ComputeFlowEntryAction = StaticCastSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry>(GraphAction);

		ActionPtr = InCreateData->Action;

		FSlateBrush const* IconBrush = FEditorStyle::GetBrush(TEXT("NoBrush"));
		GetPaletteItemIcon(GraphAction, IconBrush);

		TSharedRef<SWidget> IconWidget = CreateIconWidget(GraphAction->GetTooltipDescription(), IconBrush, FLinearColor::White);
		TSharedRef<SWidget> NameSlotWidget = CreateTextSlotWidget(NameFont, InCreateData, false); // ComputeFlowEntryAction->bIsInherited);

		// Create the actual widget
		this->ChildSlot
		[
			SNew(SHorizontalBox)
			// Icon slot
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				IconWidget
			]
			// Name slot
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(3, 0)
			[
				NameSlotWidget
			]
		];
	}

private:
	void GetPaletteItemIcon(TSharedPtr<FEdGraphSchemaAction> InGraphAction, FSlateBrush const*& OutIconBrush)
	{
		check(InGraphAction.IsValid());
		check(InGraphAction->GetTypeId() == FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId());
		TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> ComputeFlowAction = StaticCastSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry>(InGraphAction);

		if (ComputeFlowAction->Key.KeyType != EComputeFlowKeyType::ECFKT_None)
		{
			//OutIconBrush = FSlateIconFinder::FindIconBrushForClass(ComputeFlowAction->Key.KeyType->GetClass());
		}
	}

	virtual TSharedRef<SWidget> CreateTextSlotWidget(const FSlateFontInfo& NameFont, FCreateWidgetForActionData* const InCreateData, TAttribute<bool> bInIsReadOnly) override
	{
		check(InCreateData);

		TSharedPtr< SWidget > DisplayWidget;

		// Copy the mouse delegate binding if we want it
		if (InCreateData->bHandleMouseButtonDown)
		{
			MouseButtonDownDelegate = InCreateData->MouseButtonDownDelegate;
		}

		// If the creation data says read only, then it must be read only
		bIsReadOnly = InCreateData->bIsReadOnly || bInIsReadOnly.Get();

		InlineRenameWidget =
			SAssignNew(DisplayWidget, SInlineEditableTextBlock)
			.Text(this, &SFroxComputeFlowPropsItem::GetDisplayText)
			.Font(NameFont)
			.HighlightText(InCreateData->HighlightText)
			.ToolTipText(this, &SFroxComputeFlowPropsItem::GetItemTooltip)
			.OnTextCommitted(this, &SFroxComputeFlowPropsItem::OnNameTextCommitted)
			.OnVerifyTextChanged(this, &SFroxComputeFlowPropsItem::OnNameTextVerifyChanged)
			.IsSelected(InCreateData->IsRowSelectedDelegate)
			.IsReadOnly(this, &SFroxComputeFlowPropsItem::IsReadOnly);

		InCreateData->OnRenameRequest->BindSP(InlineRenameWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);

		return DisplayWidget.ToSharedRef();
	}

	virtual FText GetItemTooltip() const override
	{
		return ActionPtr.Pin()->GetTooltipDescription();
	}

	virtual void OnNameTextCommitted(const FText& NewText, ETextCommit::Type InTextCommit) override
	{
		check(ActionPtr.Pin()->GetTypeId() == FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId());

		const FString AsString = *NewText.ToString();

		if (AsString.Len() > NAME_SIZE)
		{
			UE_LOG(LogComputePropsView, Error, TEXT("%s is not a valid ComputeFlow key name. Needs to be shorter than 1024 characters."), *NewText.ToString());
			return;
		}

		TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> ComputeFlowEntryAction = StaticCastSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry>(ActionPtr.Pin());

		FName OldName = ComputeFlowEntryAction->Key.EntryName;
		FName NewName = FName(*AsString);
		if (NewName != OldName)
		{
			if (!ComputeFlowEntryAction->bIsNew)
			{
				// Preload compute flow before we transact otherwise they will add objects to 
				// the transaction buffer whether we change them or not.
				// Blueprint regeneration does this in UEdGraphNode::CreatePin.
				LoadAllComputeFlows();
			}

			const FScopedTransaction Transaction(LOCTEXT("ComputeFLowEntryRenameTransaction", "Rename ComputeFlow Entry"));
			ComputeFlowEntryAction->ComputeFlow->SetFlags(RF_Transactional);
			ComputeFlowEntryAction->ComputeFlow->Modify();
			ComputeFlowEntryAction->Key.EntryName = NewName;

			ComputeFlowEntryAction->Update();

			OnComputeFlowKeyChanged.ExecuteIfBound(ComputeFlowEntryAction->ComputeFlow, &ComputeFlowEntryAction->Key);

			if (!ComputeFlowEntryAction->bIsNew)
			{
				UpdateExternalComputeFlowKeyReferences(OldName, NewName);
			}
		}

		ComputeFlowEntryAction->bIsNew = false;
	}

	void LoadAllComputeFlows()
	{
		FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		FARFilter Filter;
		Filter.ClassNames.Add(UFroxComputeFlowAsset::StaticClass()->GetFName());
		Filter.bRecursiveClasses = true;

		TArray<FAssetData> AssetData;
		AssetRegistry.Get().GetAssets(Filter, AssetData);

		FScopedSlowTask SlowTask((float)AssetData.Num(), LOCTEXT("UpdatingComputeFlows", "Updating compute flows"));
		SlowTask.MakeDialog();

		for (const auto& ComputeFlowAsset : AssetData)
		{
			SlowTask.EnterProgressFrame(1.0f, FText::Format(LOCTEXT("CheckingComputeFlow", "Key renamed, loading {0}"), FText::FromName(ComputeFlowAsset.AssetName)));
			ComputeFlowAsset.GetAsset();
		}
	}

#define GET_STRUCT_NAME_CHECKED(StructName) \
		((void)sizeof(StructName), TEXT(#StructName))

	void UpdateExternalComputeFlowKeyReferences(const FName& OldKey, const FName& NewKey) const
	{
		// update all compute flows that reference this key
		check(ActionPtr.Pin()->GetTypeId() == FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId());
		TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> ComputeFlowEntryAction = StaticCastSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry>(ActionPtr.Pin());

		FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		FARFilter Filter;
		Filter.ClassNames.Add(UFroxComputeFlowAsset::StaticClass()->GetFName());
		Filter.bRecursiveClasses = true;

		TArray<FAssetData> AssetData;
		AssetRegistry.Get().GetAssets(Filter, AssetData);

		/*
		for (const auto& BehaviorTreeAsset : AssetData)
		{
			UBehaviorTree* BehaviorTree = Cast<UBehaviorTree>(BehaviorTreeAsset.GetAsset());
			if (BehaviorTree)
			{
				if (BehaviorTree->BlackboardAsset == ComputeFlowEntryAction->ComputeFlow)
				{
					UPackage* BehaviorTreePackage = BehaviorTreeAsset.GetPackage();
					if (BehaviorTreePackage)
					{
						// search all subobjects of this package for FBlackboardKeySelector structs and update as necessary
						TArray<UObject*> Objects;
						GetObjectsWithOuter(BehaviorTreePackage, Objects);
						for (const auto& SubObject : Objects)
						{
							for (UProperty* Property = SubObject->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
							{
								uint8* PropertyData = Property->ContainerPtrToValuePtr<uint8>(SubObject);
								UStructProperty* StructProperty = Cast<UStructProperty>(Property);

								if (StructProperty && StructProperty->GetCPPType(NULL, CPPF_None).Contains(GET_STRUCT_NAME_CHECKED(FBlackboardKeySelector)))
								{
									FBlackboardKeySelector* PropertyValue = (FBlackboardKeySelector*)PropertyData;
									if (PropertyValue)
									{
										if (PropertyValue->SelectedKeyName == OldKey)
										{
											SubObject->Modify();
											PropertyValue->SelectedKeyName = NewKey;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		*/
	}

	virtual bool OnNameTextVerifyChanged(const FText& InNewText, FText& OutErrorMessage) override
	{
		check(ActionPtr.Pin()->GetTypeId() == FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId());
		TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> ComputeFlowEntryAction = StaticCastSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry>(ActionPtr.Pin());

		const FString NewTextAsString = InNewText.ToString();

		// check for duplicate keys
		for (const auto& Key : ComputeFlowEntryAction->ComputeFlow->Keys)
		{
			if (&ComputeFlowEntryAction->Key != &Key && Key.EntryName.ToString() == NewTextAsString)
			{
				OutErrorMessage = LOCTEXT("DuplicateKeyWarning", "A key of this name already exists.");
				return false;
			}
		}

		/*
		for (const auto& Key : ComputeFlowEntryAction->ComputeFlow->ParentKeys)
		{
			if (&ComputeFlowEntryAction->Key != &Key && Key.EntryName.ToString() == NewTextAsString)
			{
				OutErrorMessage = LOCTEXT("DuplicateParentKeyWarning", "An inherited key of this name already exists.");
				return false;
			}
		}
		*/

		return true;
	}

	bool IsReadOnly() const
	{
		return bIsReadOnly;
	}

private:
	/** Delegate for when a ComputeFlow key changes (added, removed, renamed) */
	FOnComputeFlowKeyChanged OnComputeFlowKeyChanged;

	/** Read-only flag */
	bool bIsReadOnly;
};


void SFroxComputePropsView::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (ComputeFlow != nullptr)
	{
		Collector.AddReferencedObject(ComputeFlow);
	}
}

void SFroxComputePropsView::Construct(const FArguments& InArgs, TSharedRef<FUICommandList> InCommandList, UFroxComputeFlowAsset* InComputeFlow)
{
	OnEntrySelected = InArgs._OnEntrySelected;
	OnComputeFlowKeyChanged = InArgs._OnComputeFlowKeyChanged;

	ComputeFlow = InComputeFlow;

	TSharedRef<FUICommandList> CommandList = MakeShareable(new FUICommandList);

	InCommandList->Append(CommandList);

	// build debug toolbar
	FToolBarBuilder ToolbarBuilder(CommandList, FMultiBoxCustomization::None, GetToolbarExtender(InCommandList));

	ToolbarBuilder.BeginSection(TEXT("Debugging"));
	{
	}
	ToolbarBuilder.EndSection();


	ChildSlot
		[
			SNew(SBorder)
			.Padding(4.0f)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 0.0f, 0.0f, 4.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						ToolbarBuilder.MakeWidget()
					]
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SAssignNew(GraphActionMenu, SGraphActionMenu, InArgs._IsReadOnly)
					.OnCreateWidgetForAction(this, &SFroxComputePropsView::HandleCreateWidgetForAction)
					.OnCollectAllActions(this, &SFroxComputePropsView::HandleCollectAllActions)
					.OnGetSectionTitle(this, &SFroxComputePropsView::HandleGetSectionTitle)
					.OnActionSelected(this, &SFroxComputePropsView::HandleActionSelected)
					.OnContextMenuOpening(this, &SFroxComputePropsView::HandleContextMenuOpening, InCommandList)
					.OnActionMatchesName(this, &SFroxComputePropsView::HandleActionMatchesName)
					.AlphaSortItems(false)
				]
			]
		];
}

TSharedRef<SWidget> SFroxComputePropsView::HandleCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
	return SNew(SFroxComputeFlowPropsItem, InCreateData)
		.OnComputeFlowKeyChanged(OnComputeFlowKeyChanged);
}

void SFroxComputePropsView::HandleCollectAllActions(FGraphActionListBuilderBase& GraphActionListBuilder)
{
	if (ComputeFlow != nullptr)
	{
		/*
		for (auto& ParentKey : ComputeFlow->ParentKeys)
		{
			GraphActionListBuilder.AddAction(MakeShareable(new FEdGraphSchemaAction_ComputeFlowEntry(ComputeFlow, ParentKey, true)));
		}
		*/

		for (auto& Key : ComputeFlow->Keys)
		{
			GraphActionListBuilder.AddAction(MakeShareable(new FEdGraphSchemaAction_ComputeFlowEntry(ComputeFlow, Key, false)));
		}
	}
}

FText SFroxComputePropsView::HandleGetSectionTitle(int32 SectionID) const
{
	switch (SectionID)
	{
	case EComputePropsSectionTitles::InheritedKeys:
		return LOCTEXT("InheritedKeysSectionLabel", "Inherited Keys");
	case EComputePropsSectionTitles::Keys:
		return LOCTEXT("KeysSectionLabel", "Keys");
	}

	return FText();
}

void SFroxComputePropsView::HandleActionSelected(const TArray< TSharedPtr<FEdGraphSchemaAction> >& SelectedActions, ESelectInfo::Type InSelectionType) const
{
	if (InSelectionType == ESelectInfo::OnMouseClick || InSelectionType == ESelectInfo::OnKeyPress || SelectedActions.Num() == 0)
	{
		if (SelectedActions.Num() > 0)
		{
			check(SelectedActions[0]->GetTypeId() == FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId());
			TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> ComputeFlowEntry = StaticCastSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry>(SelectedActions[0]);
			OnEntrySelected.ExecuteIfBound(&ComputeFlowEntry->Key, false); //ComputeFlowEntry->bIsInherited);
		}
	}
}

TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> SFroxComputePropsView::GetSelectedEntryInternal() const
{
	TArray< TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
	GraphActionMenu->GetSelectedActions(SelectedActions);

	if (SelectedActions.Num() > 0)
	{
		check(SelectedActions[0]->GetTypeId() == FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId());
		return StaticCastSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry>(SelectedActions[0]);
	}

	return TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry>();
}

int32 SFroxComputePropsView::GetSelectedEntryIndex(bool& bOutIsInherited) const
{
	TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> Entry = GetSelectedEntryInternal();
	if (Entry.IsValid())
	{
		bOutIsInherited = false; // Entry->bIsInherited;
		FComputeFlowEntry* ComputeFlowEntry = &Entry->Key;

		// check to see what entry index we are using
		// TArray<FComputeFlowEntry>& EntryArray = bOutIsInherited ? ComputeFlow->ParentKeys : ComputeFlow->Keys;
		TArray<FComputeFlowEntry>& EntryArray = ComputeFlow->Keys;
		for (int32 Index = 0; Index < EntryArray.Num(); Index++)
		{
			if (ComputeFlowEntry == &EntryArray[Index])
			{
				return Index;
			}
		}
	}

	return INDEX_NONE;
}

FComputeFlowEntry* SFroxComputePropsView::GetSelectedEntry(bool& bOutIsInherited) const
{
	TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> Entry = GetSelectedEntryInternal();
	if (Entry.IsValid())
	{
		bOutIsInherited = false; // Entry->bIsInherited;
		return &Entry->Key;
	}

	return nullptr;
}

void SFroxComputePropsView::SetObject(UFroxComputeFlowAsset* InComputeFlow)
{
	ComputeFlow = InComputeFlow;
	GraphActionMenu->RefreshAllActions(true);
}

TSharedPtr<SWidget> SFroxComputePropsView::HandleContextMenuOpening(TSharedRef<FUICommandList> ToolkitCommands) const
{
	FMenuBuilder MenuBuilder(/* bInShouldCloseWindowAfterMenuSelection =*/true, ToolkitCommands);

	FillContextMenu(MenuBuilder);

	return MenuBuilder.MakeWidget();
}

void SFroxComputePropsView::FillContextMenu(FMenuBuilder& MenuBuilder) const
{

}

TSharedPtr<FExtender> SFroxComputePropsView::GetToolbarExtender(TSharedRef<FUICommandList> ToolkitCommands) const
{
	return TSharedPtr<FExtender>();
}

bool SFroxComputePropsView::HasSelectedItems() const
{
	bool bIsInherited = false;
	return GetSelectedEntry(bIsInherited) != nullptr;
}

bool SFroxComputePropsView::HandleActionMatchesName(FEdGraphSchemaAction* InAction, const FName& InName) const
{
	check(InAction->GetTypeId() == FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId());
	FEdGraphSchemaAction_ComputeFlowEntry* ComputeFlowAction = static_cast<FEdGraphSchemaAction_ComputeFlowEntry*>(InAction);
	return ComputeFlowAction->Key.EntryName == InName;
}

#undef LOCTEXT_NAMESPACE
