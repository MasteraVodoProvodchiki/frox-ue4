#include "SFroxComputePropsEditor.h"
#include "FroxComputeFlowAsset.h"
#include "ComputeFlowKeyType.h"
#include "FroxComputeFlowCommands.h"

#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Commands/GenericCommands.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Modules/ModuleManager.h"
#include "EditorStyleSet.h"
#include "ClassViewerFilter.h"
#include "ScopedTransaction.h"
#include "ClassViewerModule.h"
#include "SGraphActionMenu.h"

#define LOCTEXT_NAMESPACE "SFroxComputePropsEditor"

DEFINE_LOG_CATEGORY(LogComputeFlowEditor);

void SFroxComputePropsEditor::Construct(const FArguments& InArgs, TSharedRef<FUICommandList> InCommandList, UFroxComputeFlowAsset* InComputeFlow)
{
	OnEntrySelected = InArgs._OnEntrySelected;
	OnIsComputeFlowModeActive = InArgs._OnIsComputeFlowModeActive;

	// Commands
	TSharedRef<FUICommandList> CommandList = MakeShareable(new FUICommandList);

	CommandList->MapAction(
		FFroxComputeFlowPropsCommands::Get().DeleteEntry,
		FExecuteAction::CreateSP(this, &SFroxComputePropsEditor::HandleDeleteEntry),
		FCanExecuteAction::CreateSP(this, &SFroxComputePropsEditor::CanDeleteEntry)
	);

	CommandList->MapAction(
		FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &SFroxComputePropsEditor::HandleRenameEntry),
		FCanExecuteAction::CreateSP(this, &SFroxComputePropsEditor::CanRenameEntry)
	);

	InCommandList->Append(CommandList);

	// Super Construct
	SFroxComputePropsView::Construct(
		SFroxComputePropsView::FArguments()
		.OnEntrySelected(InArgs._OnEntrySelected)
		.OnComputeFlowKeyChanged(InArgs._OnComputeFlowKeyChanged)
		.IsReadOnly(false),
		CommandList,
		InComputeFlow
	);
}

void SFroxComputePropsEditor::FillContextMenu(FMenuBuilder& MenuBuilder) const
{
	if (HasSelectedItems())
	{
		MenuBuilder.AddMenuEntry(FFroxComputeFlowPropsCommands::Get().DeleteEntry);
		MenuBuilder.AddMenuEntry(FGenericCommands::Get().Rename, NAME_None, LOCTEXT("Rename", "Rename"), LOCTEXT("Rename_Tooltip", "Renames this computeflow entry."));
	}
}

void SFroxComputePropsEditor::FillToolbar(FToolBarBuilder& ToolbarBuilder) const
{
	ToolbarBuilder.AddComboButton(
		FUIAction(
			FExecuteAction(),
			FCanExecuteAction::CreateSP(this, &SFroxComputePropsEditor::CanCreateNewEntry)
		),
		FOnGetContent::CreateSP(this, &SFroxComputePropsEditor::HandleCreateNewEntryMenu),
		LOCTEXT("New_Label", "New Key"),
		LOCTEXT("New_ToolTip", "Create a new computeflow entry"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "FroxEditor.ComputeProps.NewEntry")
	);
}

TSharedPtr<FExtender> SFroxComputePropsEditor::GetToolbarExtender(TSharedRef<FUICommandList> ToolkitCommands) const
{
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Debugging", EExtensionHook::Before, ToolkitCommands, FToolBarExtensionDelegate::CreateSP(this, &SFroxComputePropsEditor::FillToolbar));

	return ToolbarExtender;
}

void SFroxComputePropsEditor::HandleDeleteEntry()
{
	if (ComputeFlow == nullptr)
	{
		UE_LOG(LogComputeFlowEditor, Error, TEXT("Trying to delete an entry from a computeflow while no Computeflow Asset is set!"));
		return;
	}

	bool bIsInherited = false;
	FComputeFlowEntry* ComputeFlowEntry = GetSelectedEntry(bIsInherited);
	if (ComputeFlowEntry != nullptr && !bIsInherited)
	{
		const FScopedTransaction Transaction(LOCTEXT("ComputeFlowEntryDeleteTransaction", "Delete ComputeFlow Entry"));
		ComputeFlow->SetFlags(RF_Transactional);
		ComputeFlow->Modify();

		for (int32 ItemIndex = 0; ItemIndex < ComputeFlow->Keys.Num(); ItemIndex++)
		{
			if (ComputeFlowEntry == &ComputeFlow->Keys[ItemIndex])
			{
				ComputeFlow->Keys.RemoveAt(ItemIndex);
				break;
			}
		}

		GraphActionMenu->RefreshAllActions(true);
		OnComputeFlowKeyChanged.ExecuteIfBound(ComputeFlow, nullptr);

		// signal de-selection
		if (OnEntrySelected.IsBound())
		{
			OnEntrySelected.Execute(nullptr, false);
		}
	}
}

void SFroxComputePropsEditor::HandleRenameEntry()
{
	GraphActionMenu->OnRequestRenameOnActionNode();
}

bool SFroxComputePropsEditor::CanDeleteEntry() const
{
	const bool bModeActive = OnIsComputeFlowModeActive.IsBound() && OnIsComputeFlowModeActive.Execute();

	if (bModeActive)
	{
		bool bIsInherited = false;
		FComputeFlowEntry* ComputeFlowEntry = GetSelectedEntry(bIsInherited);
		if (ComputeFlowEntry != nullptr)
		{
			return !bIsInherited;
		}
	}

	return false;
}

bool SFroxComputePropsEditor::CanRenameEntry() const
{
	const bool bModeActive = OnIsComputeFlowModeActive.IsBound() && OnIsComputeFlowModeActive.Execute();

	if (bModeActive)
	{
		bool bIsInherited = false;
		FComputeFlowEntry* ComputeFlowEntry = GetSelectedEntry(bIsInherited);
		if (ComputeFlowEntry != nullptr)
		{
			return !bIsInherited;
		}
	}

	return false;
}

class FComputeFlowEntryClassFilter : public IClassViewerFilter
{
public:

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		if (InClass != nullptr)
		{
			return !InClass->HasAnyClassFlags(CLASS_Abstract | CLASS_HideDropDown) &&
				InClass->HasAnyClassFlags(CLASS_EditInlineNew) &&
				InClass->IsChildOf(UComputeFlowKeyType::StaticClass());
		}
		return false;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return InUnloadedClassData->IsChildOf(UComputeFlowKeyType::StaticClass());
	}
};


TSharedRef<SWidget> SFroxComputePropsEditor::HandleCreateNewEntryMenu() const
{
	FClassViewerInitializationOptions Options;
	Options.bShowUnloadedBlueprints = true;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::DisplayName;
	Options.ClassFilter = MakeShareable(new FComputeFlowEntryClassFilter);

	FOnClassPicked OnPicked(FOnClassPicked::CreateRaw(this, &SFroxComputePropsEditor::HandleKeyClassPicked));

	// clear the search box, just in case there's something typed in there 
	// We need to do that since key adding code takes advantage of selection mechanics
	TSharedRef<SEditableTextBox> FilterTextBox = GraphActionMenu->GetFilterTextBox();
	FilterTextBox->SetText(FText());

	return
		SNew(SBox)
		.HeightOverride(240.0f)
		.WidthOverride(200.0f)
		[
			FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(Options, OnPicked)
		];
}

void SFroxComputePropsEditor::HandleKeyClassPicked(UClass* InClass)
{
	if (ComputeFlow == nullptr)
	{
		UE_LOG(LogComputeFlowEditor, Error, TEXT("Trying to delete an entry from a computeflow while no ComputeFlow Asset is set!"));
		return;
	}

	FSlateApplication::Get().DismissAllMenus();

	check(InClass);
	check(InClass->IsChildOf(UComputeFlowKeyType::StaticClass()));

	const FScopedTransaction Transaction(LOCTEXT("ComputeFlowdEntryAddTransaction", "Add ComputeFlow Entry"));
	ComputeFlow->SetFlags(RF_Transactional);
	ComputeFlow->Modify();

	// create a name for this new key
	FString NewKeyName = InClass->GetDisplayNameText().ToString();
	NewKeyName = NewKeyName.Replace(TEXT(" "), TEXT(""));
	NewKeyName += TEXT("Key");

	int32 IndexSuffix = -1;
	auto DuplicateFunction = [&](const FComputeFlowEntry& Key)
	{
		if (Key.EntryName.ToString() == NewKeyName)
		{
			IndexSuffix = FMath::Max(0, IndexSuffix);
		}
		if (Key.EntryName.ToString().StartsWith(NewKeyName))
		{
			const FString ExistingSuffix = Key.EntryName.ToString().RightChop(NewKeyName.Len());
			if (ExistingSuffix.IsNumeric())
			{
				IndexSuffix = FMath::Max(FCString::Atoi(*ExistingSuffix) + 1, IndexSuffix);
			}
		}
	};

	// check for existing keys of the same name
	for (const auto& Key : ComputeFlow->Keys) { DuplicateFunction(Key); };
	// for (const auto& Key : ComputeFlow->ParentKeys) { DuplicateFunction(Key); };

	if (IndexSuffix != -1)
	{
		NewKeyName += FString::Printf(TEXT("%d"), IndexSuffix);
	}

	FComputeFlowEntry Entry;
	Entry.EntryName = FName(*NewKeyName);
	Entry.KeyType = NewObject<UComputeFlowKeyType>(ComputeFlow, InClass);

	ComputeFlow->Keys.Add(Entry);

	GraphActionMenu->RefreshAllActions(true);
	OnComputeFlowKeyChanged.ExecuteIfBound(ComputeFlow, &ComputeFlow->Keys.Last());

	GraphActionMenu->SelectItemByName(Entry.EntryName, ESelectInfo::OnMouseClick);

	// Mark newly created entry as 'new'
	TArray< TSharedPtr<FEdGraphSchemaAction> > SelectedActions;
	GraphActionMenu->GetSelectedActions(SelectedActions);
	check(SelectedActions.Num() == 1);
	check(SelectedActions[0]->GetTypeId() == FEdGraphSchemaAction_ComputeFlowEntry::StaticGetTypeId());
	TSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry> ComputeFlowEntryAction = StaticCastSharedPtr<FEdGraphSchemaAction_ComputeFlowEntry>(SelectedActions[0]);
	ComputeFlowEntryAction->bIsNew = true;

	GraphActionMenu->OnRequestRenameOnActionNode();
}

bool SFroxComputePropsEditor::CanCreateNewEntry() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE
