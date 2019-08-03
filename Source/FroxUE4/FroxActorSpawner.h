// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FroxActorSpawner.generated.h"

class UFroxComputeData;

UCLASS()
class FROXUE4_API AFroxActorSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFroxActorSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Frox|Components")
	void SetComputeData(UFroxComputeData* InComputeData);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Frox)
	UFroxComputeData* ComputeData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Frox)
	UClass* SpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Frox)
	FVector Scale = FVector(1.f);

private:
	UPROPERTY()
	TArray<AActor*> SpawnedActors;

private:
	void ClearActor();
	void UpdateActors();
};
