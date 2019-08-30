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

	UPROPERTY()
	FGuid UniqueId;
#endif // WITH_EDITORONLY_DATA

	/** key type and additional properties */
	UPROPERTY(EditAnywhere, Category = Frox)
	EComputeFlowKeyType KeyType;

	UPROPERTY(EditAnywhere, Category = Frox)
	EComputeFlowEntryDirection Direction;

	FComputeFlowEntry()
		: UniqueId(FGuid::NewGuid()),
		KeyType(EComputeFlowKeyType::ECFKT_None)
	{}

	bool operator==(const FComputeFlowEntry& Other) const;
};

UCLASS(BlueprintType, ClassGroup = (Frox))
class FROXPLUGIN_API UFroxComputeFlowAsset : public UObject
{
	GENERATED_BODY()

	using NodePair = TKeyValuePair<UOpartionNode*, frox::ComputeNode*>;

public:
	/** BEGIN UObject overrides */
	virtual void BeginDestroy() override;
	// End UObject

	frox::ComputeFlow* CreateFlow() const;
	FName GetComputeFlowName() const;

	bool FindEntry(FName EntryName, FComputeFlowEntry& Out) const;

	UPROPERTY()
	class UEdGraph* UpdateGraph;

	/** computeflow keys */
	UPROPERTY(EditAnywhere, Category=Frox)
	TArray<FComputeFlowEntry> Keys;

protected:
	UPROPERTY()
	FName computeFlowFileName;

private:
	void InitializeFlowOperations(frox::ComputeFlow* ComputeFlow, const TArray<UOpartionNode*>& Operations, const TArray<NodePair>& Pairs) const;
	void InitializeFlowInputs(frox::ComputeFlow* ComputeFlow, const UInputPropertyNode* Inputs, const TArray<NodePair>& Pairs) const;
	void InitializeFlowOutputs(frox::ComputeFlow* ComputeFlow, const UOutputPropertyNode* Outputs, const TArray<NodePair>& Pairs) const;

#if WITH_EDITORONLY_DATA
	virtual void PostLoad() override;
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