#pragma once

#include "ComputeFlowKeyType.h"

#include "FroxComputeFlowAsset.generated.h"

namespace frox {
	class ComputeFlow;
	class IComputeFlowListerner;
	class ComputeNode;
} // End frox

class UInputPropertyNode;
class UOutputPropertyNode;
class UOpartionNode;


UENUM(BlueprintType)
enum class EComputeFlowEntryDirection : uint8
{
	ECFED_Input,
	ECFED_Output,
};

USTRUCT()
struct FComputeFlowEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Frox)
	FName EntryName;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Frox, Meta = (ToolTip = "Optional description."))
	FString EntryDescription;
#endif // WITH_EDITORONLY_DATA

	/** key type and additional properties */
	UPROPERTY(EditAnywhere, Instanced, Category = Frox)
	UComputeFlowKeyType* KeyType;

	UPROPERTY(EditAnywhere, Category = Frox)
	EComputeFlowEntryDirection Direction;

	FComputeFlowEntry()
		: KeyType(nullptr)
	{}

	bool operator==(const FComputeFlowEntry& Other) const;
};

UCLASS(BlueprintType, ClassGroup = (Frox))
class FROXPLUGIN_API UFroxComputeFlowAsset : public UObject
{
	GENERATED_BODY()

	using NodePair = TKeyValuePair<UOpartionNode*, frox::ComputeNode*>;

public:
	virtual void BeginDestroy() override;

	frox::ComputeFlow* CreateFlow(frox::IComputeFlowListerner* Listerner = nullptr) const;
	void InitializeFlowOperations(frox::ComputeFlow* ComputeFlow, const TArray<UOpartionNode*>& Operations, const TArray<NodePair>& Pairs) const;
	void InitializeFlowInputs(frox::ComputeFlow* ComputeFlow, const TArray<UInputPropertyNode*>& Inputs, const TArray<NodePair>& Pairs) const;
	void InitializeFlowOutputs(frox::ComputeFlow* ComputeFlow, const TArray<UOutputPropertyNode*>& Outputs, const TArray<NodePair>& Pairs) const;

	FName GetComputeFlowName() const;

	UPROPERTY()
	class UEdGraph* UpdateGraph;

	/** computeflow keys */
	UPROPERTY(EditAnywhere, Category=Frox)
	TArray<FComputeFlowEntry> Keys;

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