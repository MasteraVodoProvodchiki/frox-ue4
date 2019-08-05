// Fill out your copyright notice in the Description page of Project Settings.


#include "FroxActorSpawner.h"
#include "FroxComputeData.h"
#include "Engine/World.h"

#include "Frox/Frox/RadiusOfContourComputeNode.h"

// Sets default values
AFroxActorSpawner::AFroxActorSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFroxActorSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFroxActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFroxActorSpawner::SetComputeData(UFroxComputeData* InComputeData)
{
	ComputeData = InComputeData;
	
	if (ComputeData != nullptr && SpawnClass != nullptr)
	{
		UpdateActors();
	}
	else
	{
		ClearActor();
	}
}

void AFroxActorSpawner::ClearActor()
{
	for (AActor* Actor : SpawnedActors)
	{
		Actor->Destroy();
	}
}

void AFroxActorSpawner::UpdateActors()
{
	check(ComputeData != nullptr && SpawnClass != nullptr);

	frox::ComputeDataPtr FroxComputeData = ComputeData->GetFroxData();
	frox::RadiusOfContourComputeData* RadiusOfContourData = FroxComputeData->As<frox::RadiusOfContourComputeData>();

	if (RadiusOfContourData != nullptr && SpawnedActors.Num() != RadiusOfContourData->Values.size())
	{
		for (auto& value : RadiusOfContourData->Values)
		{
			FTransform Transform = GetActorTransform();
			FTransform LocalTransform = FTransform(
				FRotator::ZeroRotator,
				FVector{
					float(value.Center.X),
					float(value.Center.Y),
					0.f
				} * Scale,
				FVector(value.Radius)
			);
			
			FTransform WorldTransform = LocalTransform * Transform;
			AActor* SpawnedActor = GetWorld()->SpawnActor(SpawnClass, &WorldTransform);
			SpawnedActors.Add(SpawnedActor);

			SpawnedActor->SetActorScale3D(FVector(value.Radius));
		}
	}
}