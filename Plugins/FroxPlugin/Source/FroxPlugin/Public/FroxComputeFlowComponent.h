#pragma once

#include "Components/ActorComponent.h"
#include "Frox/Frox/Frox.h"

#include "FroxComputeFlowComponent.generated.h"

class UFroxComputeFlowAsset;

UCLASS(ClassGroup = (Frox), meta = (BlueprintSpawnableComponent))
class FROXPLUGIN_API UFroxComputeFlowComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

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
	void SetValueAsInt(const FName& KeyName = NAME_None, int32 IntValue = 0);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsFloat(const FName& KeyName = NAME_None, float FloatValue = 0.f);

	UFUNCTION(BlueprintCallable, Category="Frox|Components|Data")
	void SetValueAsBool(const FName& KeyName = NAME_None, bool BoolValue = true);

private:
	/** setup component for using given computeflow asset */
	bool InitializeFlow(UFroxComputeFlowAsset& NewAsset);
	void ReleaseFlow();

private:
	frox::ComputeFlow* ComputeFlow;

	bool _bRunning = true;

	// TODO. Add ValuesMem and Values Offset instead of maps
	TMap<FName, int32> IntValues;
	TMap<FName, float> FloatValues;
	TMap<FName, bool> BoolValues;
};