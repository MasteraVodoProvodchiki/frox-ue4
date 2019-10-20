// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "DyanmicGridMeshComponent.generated.h"

class UFroxComputeFrame;
/**
 * 
 */
UCLASS(ClassGroup = (Frox), meta = (BlueprintSpawnableComponent))
class FROXUE4_API UDyanmicGridMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
#endif
	
	UFUNCTION(BlueprintCallable, Category="Frox|Components")
	void SetHeightMap(UFroxComputeFrame* InHeightMap);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Frox)
	UFroxComputeFrame* HeightMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Frox)
	FVector PolygonScale = FVector(100.f);

private:
	void UpdateLandscapeMesh();
	void UpdateLandscapeMesh(uint32 HeightmapSizeX, uint32 HeightmapSizeY, const uint16_t* Heightmap);
	void UpdateLandscapeNavigationGrid();

private:
	uint32 LastHeightmapSizeX = 0;
	uint32 LastHeightmapSizeY = 0;

	TArray<int32> Triangles;
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

};
