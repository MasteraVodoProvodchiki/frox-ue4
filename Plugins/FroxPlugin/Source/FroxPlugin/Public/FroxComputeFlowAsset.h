#pragma once

#include "FroxComputeFlowAsset.generated.h"

UCLASS(BlueprintType, ClassGroup = (Frox))
class FROXPLUGIN_API UFroxComputeFlowAsset : public UObject
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;

	FName GetComputeFlowName() const;

	UPROPERTY()
	class UEdGraph* UpdateGraph;

protected:
	UPROPERTY()
	FName computeFlowFileName;

#if WITH_EDITORONLY_DATA

public:
	void SetComputeFlowName(const FName &ComputeFlowFileName);

protected:
	UPROPERTY(VisibleAnywhere, Instanced, Category = ImportSettings)
	class UAssetImportData* importData;

	virtual void PostInitProperties() override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	virtual void Serialize(FArchive& Ar) override;
#endif
};