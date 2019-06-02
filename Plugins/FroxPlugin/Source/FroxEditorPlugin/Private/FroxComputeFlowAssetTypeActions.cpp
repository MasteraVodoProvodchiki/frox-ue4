#include "FroxComputeFlowAssetTypeActions.h"
#include "FroxComputeFlowAsset.h"
#include "FroxComputeFlowAssetEditor.h"

#define LOCTEXT_NAMESPACE "FroxComputeFlowAssetTypeActions"

FFroxComputeFlowAssetTypeActions::FFroxComputeFlowAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
	:MyAssetCategory(InAssetCategory)
{

}

FText FFroxComputeFlowAssetTypeActions::GetName() const
{
	return LOCTEXT("FroxComputeFlowAssetName", "Frox ComputeFlow Asset");
}

UClass* FFroxComputeFlowAssetTypeActions::GetSupportedClass() const
{
	return UFroxComputeFlowAsset::StaticClass();
}

FColor FFroxComputeFlowAssetTypeActions::GetTypeColor() const
{
	return FColorList::BrightGold;
}

void FFroxComputeFlowAssetTypeActions::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (UObject *obj : TypeAssets)
	{
		UFroxComputeFlowAsset *asset = Cast<UFroxComputeFlowAsset>(obj);

		if (asset != nullptr)
		{
			/*
			const FString &filename = animAsset->GetCreatureFilename();
			if (!filename.IsEmpty())
			{
				OutSourceFilePaths.Add(filename);
			}
			*/
		}
	}
}

uint32 FFroxComputeFlowAssetTypeActions::GetCategories()
{
	return MyAssetCategory;
}

void FFroxComputeFlowAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UFroxComputeFlowAsset* PropData = Cast<UFroxComputeFlowAsset>(*ObjIt);
		if (PropData)
		{
			TSharedRef<FFroxComputeFlowAssetEditor> NewCustEditor(new FFroxComputeFlowAssetEditor());

			NewCustEditor->InitCustAssetEditor(Mode, EditWithinLevelEditor, PropData);
		}
	}
}

void FFroxComputeFlowAssetTypeActions::RegistrateCustomPartAssetType()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	EAssetTypeCategories::Type froxComputeFlowAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("FroxAssetCategory")), LOCTEXT("FroxAssetCategory", "Frox"));

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FFroxComputeFlowAssetTypeActions(froxComputeFlowAssetCategoryBit)));
}

#undef LOCTEXT_NAMESPACE