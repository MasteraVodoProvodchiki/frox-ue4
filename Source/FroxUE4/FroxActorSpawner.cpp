// Fill out your copyright notice in the Description page of Project Settings.


#include "FroxActorSpawner.h"
#include "FroxComputeData.h"
#include "Engine/World.h"

#include "Frox/Frox/CenterOfContourComputeNode.h"

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
	frox::CenterOfContourComputeData* CenterOfContourData = FroxComputeData->As<frox::CenterOfContourComputeData>();

	if (CenterOfContourData != nullptr && SpawnedActors.Num() != CenterOfContourData->Points.size())
	{
		for (frox::Point point : CenterOfContourData->Points)
		{
			FTransform Transform = GetActorTransform();
			FTransform LocalTransform = FTransform(FVector{
				float(point.X),
				float(point.Y),
				0.f
			} * Scale);

			FTransform WorldTransform = LocalTransform * Transform;
			AActor* SpawnedActor = GetWorld()->SpawnActor(SpawnClass, &WorldTransform);
			SpawnedActors.Add(SpawnedActor);
		}
	}
}