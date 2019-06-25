#include "FroxComputeFrameAssetTypeActions.h"
#include "FroxComputeFrame.h"

#define LOCTEXT_NAMESPACE "FroxComputeFrameAssetTypeActions"

FFroxComputeFrameAssetTypeActions::FFroxComputeFrameAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
	:MyAssetCategory(InAssetCategory)
{

}

FText FFroxComputeFrameAssetTypeActions::GetName() const
{
	return LOCTEXT("FroxComputeFrameAssetName", "Frox ComputeFrame Asset");
}

UClass* FFroxComputeFrameAssetTypeActions::GetSupportedClass() const
{
	return UFroxComputeFrame::StaticClass();
}

FColor FFroxComputeFrameAssetTypeActions::GetTypeColor() const
{
	return FColorList::BrightGold;
}

void FFroxComputeFrameAssetTypeActions::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (UObject *Obj : TypeAssets)
	{
		UFroxComputeFrame *ComputeFrame = Cast<UFroxComputeFrame>(Obj);

		if (ComputeFrame != nullptr)
		{
			/*
			const FString &filename = ComputeFrame->GetComputeFlowName();
			if (!filename.IsEmpty())
			{
				OutSourceFilePaths.Add(filename);
			}
			*/
		}
	}
}

uint32 FFroxComputeFrameAssetTypeActions::GetCategories()
{
	return MyAssetCategory;
}

void FFroxComputeFrameAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{

	}
}

void FFroxComputeFrameAssetTypeActions::RegistrateCustomPartAssetType()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	EAssetTypeCategories::Type froxComputeFlowAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("FroxAssetCategory")), LOCTEXT("FroxAssetCategory", "Frox"));

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FFroxComputeFrameAssetTypeActions(froxComputeFlowAssetCategoryBit)));
}

#undef LOCTEXT_NAMESPACE