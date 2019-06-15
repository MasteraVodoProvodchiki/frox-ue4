#pragma once

#include "Components/ActorComponent.h"
#include "Frox/Frox/Frox.h"

#include "FroxComputeFlowComponent.generated.h"

class UFroxComputeFlowAsset;
class FroxComputeFrame;

class UInputPropertyNode;
class UOutputPropertyNode;
class UOpartionNode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FValueChanged, FName, Name);

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EFroxComputeFlowPerformResult : uint8
{
	None 		UMETA(DisplayName = "None"),
	InProcess	UMETA(DisplayName = "InProcess"),
	Success		UMETA(DisplayName = "Success"),
	Error 		UMETA(DisplayName = "Error")
};

USTRUCT(BlueprintType)
struct FFroxComputeFlowPerformResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = Frox)
	EFroxComputeFlowPerformResult Result;
};

UCLASS(ClassGroup = (Frox), meta = (BlueprintSpawnableComponent))
class FROXPLUGIN_API UFroxComputeFlowComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
	struct FHoleNode
	{
		frox::ComputeNode* Node;
		uint32 PinId;
	};

	struct FHole
	{
		TArray<FHoleNode> Nodes;
	};

	using NodePair = TKeyValuePair<UOpartionNode*, frox::ComputeNode*>;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Frox)
	UFroxComputeFlowAsset* ComputeFlowAsset;

	/** BEGIN UActorComponent overrides */
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	// End UActorComponent

public:
	UFUNCTION(BlueprintCallable, Category = "Frox|Components", Meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo"))
	void Perform(UObject* WorldContextObject, FLatentActionInfo LatentInfo, FFroxComputeFlowPerformResult& PerformResult);

	UFUNCTION(BlueprintCallable, Category="Frox|Components")
	void Start();

	UFUNCTION(BlueprintCallable, Category="Frox|Components")
	void Stop();

	UFUNCTION(BlueprintCallable, Category="Frox|Components")
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	int32 GetValueAsInt(const FName& KeyName = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	float GetValueAsFloat(const FName& KeyName = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	bool GetValueAsBool(const FName& KeyName = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	UFroxComputeFrame* GetValueAsFrame(const FName& KeyName = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsInt(const FName& KeyName = NAME_None, int32 IntValue = 0);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsFloat(const FName& KeyName = NAME_None, float FloatValue = 0.f);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsBool(const FName& KeyName = NAME_None, bool BoolValue = true);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsFrame(const FName& KeyName, UFroxComputeFrame* ComputeFrame);

	UPROPERTY(BlueprintAssignable, Category = "Frox|Components|Data")
	FValueChanged OnValueChanged;

	void OnPerformed();

	/** The final call related to screenshots, after they've been taken, and after they've been compared (or not if automation isn't running). */
	FSimpleMulticastDelegate OnPerformCompleted;

private:
	/** setup component for using given computeflow asset */
	bool InitializeFlow(UFroxComputeFlowAsset& NewAsset);
	void InitializeFlowOperations(const TArray<UOpartionNode*>& Operations, const TArray<NodePair>& Pairs);
	void InitializeFlowInputs(const TArray<UInputPropertyNode*>& Inputs, const TArray<NodePair>& Pairs);
	void InitializeFlowOutputs(const TArray<UOutputPropertyNode*>& Outputs, const TArray<NodePair>& Pairs);
	void ReleaseFlow();

	void FetchFlow();
	void PerformFlow();

private:
	frox::ComputeFlow* ComputeFlow;
	TSharedPtr<class FComputeFlowListerner> ComputeFlowListerner;

	bool _bRunning = false;

	// TODO. Add ValuesMem and Values Offset instead of maps
	TMap<FName, int32> IntValues;
	TMap<FName, float> FloatValues;
	TMap<FName, bool> BoolValues;

	UPROPERTY()
	TMap<FName, UFroxComputeFrame*> FrameValues;

	TMap<FName, FHole> InputHoles;
	TMap<FName, FHole> OuputHoles;
};