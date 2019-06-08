#include "FroxComputeFlowAsset.h"

#define LOCTEXT_NAMESPACE "FFroxPluginModule"

FName UFroxComputeFlowAsset::GetComputeFlowName() const
{
	return computeFlowFileName;
}

#if WITH_EDITORONLY_DATA
void UFroxComputeFlowAsset::SetComputeFlowName(const FName& ComputeFlowFileName)
{
	computeFlowFileName = ComputeFlowFileName;
}

void UFroxComputeFlowAsset::PostInitProperties()
{
	// if (!HasAnyFlags(RF_ClassDefaultObject)) importData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	Super::PostInitProperties();
}

void UFroxComputeFlowAsset::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	//if (importData) {
	//	OutTags.Add(FAssetRegistryTag(SourceFileTagName(), importData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden) );
	//}

	Super::GetAssetRegistryTags(OutTags);
}

void UFroxComputeFlowAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	//if (Ar.IsLoading() && Ar.UE4Ver() < VER_UE4_ASSET_IMPORT_DATA_AS_JSON && !importData)
	//	importData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
}

#endif

void UFroxComputeFlowAsset::BeginDestroy() {

	Super::BeginDestroy();
}

#undef LOCTEXT_NAMESPACE