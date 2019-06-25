#pragma once

#include "AssetTypeActions_Base.h"

class FFroxComputeFrameAssetTypeActions : public FAssetTypeActions_Base
{
public:
	FFroxComputeFrameAssetTypeActions(EAssetTypeCategories::Type InAssetCategory);

	// IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	virtual bool IsImportedAsset() const override
	{
		return true;
	}

	/** Collects the resolved source paths for the imported assets */
	virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;

	// End of IAssetTypeActions interface

	static void RegistrateCustomPartAssetType();

private:
	EAssetTypeCategories::Type MyAssetCategory;
};