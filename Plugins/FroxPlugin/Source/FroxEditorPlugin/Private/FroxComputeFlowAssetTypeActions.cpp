#include "FroxComputeFlowAssetTypeActions.h"
#include "FroxComputeFlowAsset.h"

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

#undef LOCTEXT_NAMESPACE