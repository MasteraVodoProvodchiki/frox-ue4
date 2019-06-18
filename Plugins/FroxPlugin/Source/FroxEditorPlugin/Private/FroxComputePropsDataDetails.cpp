// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FroxComputePropsDataDetails.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SBoxPanel.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "FroxComputeFlowAsset.h"

#define LOCTEXT_NAMESPACE "FroxComputePropsDataDetails"

TSharedRef<IDetailCustomization> FFroxComputePropsDataDetails::MakeInstance(FOnGetSelectedComputePropsItemIndex InOnGetSelectedBlackboardItemIndex)
{
	return MakeShareable(new FFroxComputePropsDataDetails(InOnGetSelectedBlackboardItemIndex));
}

void FFroxComputePropsDataDetails::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	// First hide all keys
	DetailLayout.HideProperty(TEXT("Keys"));
	DetailLayout.HideProperty(TEXT("ImportData"));

	// Now show only the currently selected key
	bool bIsInherited = false;
	int32 CurrentSelection = INDEX_NONE;
	if (OnGetSelectedBlackboardItemIndex.IsBound())
	{
		CurrentSelection = OnGetSelectedBlackboardItemIndex.Execute(bIsInherited);
	}

	if (CurrentSelection >= 0)
	{
		TSharedPtr<IPropertyHandle> KeysHandle = DetailLayout.GetProperty(TEXT("Keys"));
			// bIsInherited ? DetailLayout.GetProperty(TEXT("ParentKeys")) : DetailLayout.GetProperty(TEXT("Keys"));
		check(KeysHandle.IsValid());
		uint32 NumChildKeys = 0;
		KeysHandle->GetNumChildren(NumChildKeys);
		if ((uint32)CurrentSelection < NumChildKeys)
		{
			TSharedPtr<IPropertyHandle> KeyHandle = KeysHandle->GetChildHandle((uint32)CurrentSelection);

			IDetailCategoryBuilder& DetailCategoryBuilder = DetailLayout.EditCategory("Key");
			TSharedPtr<IPropertyHandle> EntryNameProperty = KeyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FComputeFlowEntry, EntryName));
			DetailCategoryBuilder.AddCustomRow(LOCTEXT("EntryNameLabel", "Entry Name"))
				.NameContent()
				[
					EntryNameProperty->CreatePropertyNameWidget()
				]
			.ValueContent()
				[
					SNew(SHorizontalBox)
					.IsEnabled(true)
				+ SHorizontalBox::Slot()
				[
					EntryNameProperty->CreatePropertyValueWidget()
				]
				];

#if WITH_EDITORONLY_DATA
			TSharedPtr<IPropertyHandle> EntryDescriptionHandle = KeyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FComputeFlowEntry, EntryDescription));
			DetailCategoryBuilder.AddProperty(EntryDescriptionHandle);
#endif

			TSharedPtr<IPropertyHandle> KeyTypeProperty = KeyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FComputeFlowEntry, KeyType));
			DetailCategoryBuilder.AddProperty(KeyTypeProperty);

			TSharedPtr<IPropertyHandle> DirectionProperty = KeyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FComputeFlowEntry, Direction));
			DetailCategoryBuilder.AddProperty(DirectionProperty);
		}
	}
}

#undef LOCTEXT_NAMESPACE
