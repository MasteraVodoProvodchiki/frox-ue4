#pragma once

#include "Components/ActorComponent.h"
#include "Engine/LatentActionManager.h"

#include "Frox/Frox/Frox.h"

#include "FroxComputeFlowComponent.generated.h"

class UFroxComputeFlowAsset;
class UFroxComputeFrame;
class UFroxComputeData;

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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Frox)
	UFroxComputeFlowAsset* ComputeFlowAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Frox)
	bool bAutoStart = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Frox)
	float PerformFrequency = 1000.f;

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
	FIntPoint GetValueAsPoint(const FName& KeyName = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	FVector2D GetValueAsVector2D(const FName& KeyName = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	UFroxComputeFrame* GetValueAsFrame(const FName& KeyName = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	UFroxComputeData* GetValueAsData(const FName& KeyName = NAME_None) const;

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsInt(const FName& KeyName = NAME_None, int32 IntValue = 0);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsFloat(const FName& KeyName = NAME_None, float FloatValue = 0.f);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsBool(const FName& KeyName = NAME_None, bool BoolValue = true);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsPoint(const FName& KeyName, FIntPoint PointValue);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsVector2D(const FName& KeyName, FVector2D Vector2dValue);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsFrame(const FName& KeyName, UFroxComputeFrame* ComputeFrame);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsData(const FName& KeyName, UFroxComputeData* ComputeData);

	UPROPERTY(BlueprintAssignable, Category = "Frox|Components|Data")
	FValueChanged OnValueChanged;

	void OnPerformed();

	/** The final call related to screenshots, after they've been taken, and after they've been compared (or not if automation isn't running). */
	FSimpleMulticastDelegate OnPerformCompleted;

protected:
	virtual void BeforePerformFlow();
	virtual void AfterFetchFlow();

private:
	/** setup component for using given computeflow asset */
	bool InitializeFlow(UFroxComputeFlowAsset& NewAsset);
	void ReleaseFlow();

	UFroxComputeFrame* FetchFrame(const frox::FlowData& Data, FName EntryID) const;
	UFroxComputeData* FetchData(const frox::FlowData& Data, FName EntryID) const;
	void FetchFlow();
	void PerformFlow();

private:
	frox::ComputeFlow* ComputeFlow;
	frox::FlowPerformer* Performer;
	frox::FlowData* InputData;
	frox::FlowData* OutputData;
	TSharedPtr<class FComputeFlowListerner> ComputeFlowListerner;

	bool _bRunning = false;
	float CurrentTime = 0.f;

	// TODO. Add ValuesMem and Values Offset instead of maps
	TMap<FName, int32> IntValues;
	TMap<FName, float> FloatValues;
	TMap<FName, bool> BoolValues;
	TMap<FName, FIntPoint> PointValues;
	TMap<FName, FVector2D> Vector2DValues;

	UPROPERTY()
	TMap<FName, UFroxComputeFrame*> FrameValues;

	UPROPERTY()
	TMap<FName, UFroxComputeData*> DataValues;
};