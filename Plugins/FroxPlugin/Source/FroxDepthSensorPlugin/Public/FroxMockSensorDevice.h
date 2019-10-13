// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FroxComputeFrame.h"

#include "Frox/FroxDepthSensor/MockSensorDeviceManager.h"

#include "FroxMockSensorDevice.generated.h"

UCLASS()
class FROXDEPTHSENSORPLUGIN_API AFroxMockSensorDevice : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFroxMockSensorDevice();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintSetter)
	void SetDepthFrame(UFroxComputeFrame* Frame);

	UFUNCTION(BlueprintSetter)
	void SetColorFrame(UFroxComputeFrame* Frame);

	UFUNCTION(BlueprintSetter)
	void SetInfraredFrame(UFroxComputeFrame* Frame);

private:
	void InitializeSensor();
	void ReleaseSensor();

public:
	UPROPERTY(EditAnywhere, BlueprintSetter = SetDepthFrame)
	UFroxComputeFrame* DepthFrame;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetColorFrame)
	UFroxComputeFrame* ColorFrame;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetInfraredFrame)
	UFroxComputeFrame* InfraredFrame;

private:
	frox::MockSensorDeviceManagerPtr _MockDeviceManager;
	frox::MockSensorDevicePtr _MocDevice;
};