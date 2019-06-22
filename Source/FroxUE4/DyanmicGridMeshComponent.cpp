// Fill out your copyright notice in the Description page of Project Settings.


#include "DyanmicGridMeshComponent.h"
#include "FroxComputeFrame.h"
#include "NavigationSystem.h"

void UDyanmicGridMeshComponent::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	if (
		PropertyName == GET_MEMBER_NAME_CHECKED(UDyanmicGridMeshComponent, HeightMap) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UDyanmicGridMeshComponent, PolygonScale)
	)
	{
		UpdateLandscapeMesh();
	}

	Super::PostEditChangeProperty(e);
}

void UDyanmicGridMeshComponent::SetHeightMap(UFroxComputeFrame* InHeightMap)
{
	HeightMap = InHeightMap;
	UpdateLandscapeMesh();
}

void UDyanmicGridMeshComponent::UpdateLandscapeMesh()
{
	if (HeightMap != nullptr)
	{
		frox::ComputeFramePtr FroxFrame = HeightMap->GetFroxFrame();

		frox::EComputeFrameType FroxFrameType = FroxFrame->GetType();
		frox::Size FroxFrameSize = FroxFrame->GetSize();

		if (FroxFrameType == frox::EComputeFrameType::ECFT_UInt16 && FroxFrameSize.Width > 0 && FroxFrameSize.Height > 0)
		{		
			const uint16_t* FroxFrameData = FroxFrame->GetData<uint16_t>();
			UpdateLandscapeMesh(FroxFrameSize.Width, FroxFrameSize.Height, FroxFrameData);
		}
		else
		{

		}
	}
}

void UDyanmicGridMeshComponent::UpdateLandscapeMesh(uint32 HeightmapSizeX, uint32 HeightmapSizeY, const uint16_t* Heightmap)
{
	bool bResolutionChanged = LastHeightmapSizeX != HeightmapSizeX || LastHeightmapSizeY != HeightmapSizeY;

	if(bResolutionChanged)
	{
		Triangles.SetNum(HeightmapSizeX * HeightmapSizeY * 6);
		Vertices.SetNum(HeightmapSizeX * HeightmapSizeY);
		Normals.SetNum(HeightmapSizeX * HeightmapSizeY);
		UV0.SetNum(HeightmapSizeX * HeightmapSizeY);
		Tangents.SetNum(HeightmapSizeX * HeightmapSizeY);

		this->ClearAllMeshSections();

		LastHeightmapSizeX = HeightmapSizeX;
		LastHeightmapSizeY = HeightmapSizeY;
	}

	uint32 VerticesPerRow = HeightmapSizeX + 1;

	float PositionOffsetX = float(HeightmapSizeX) * PolygonScale.X / 2;
	float PositionOffsetY = float(HeightmapSizeY) * PolygonScale.Y / 2;

	for (uint32 HeightmapY = 0; HeightmapY < HeightmapSizeY; ++HeightmapY)
	{
		float VertexPositionY = float(HeightmapY) * PolygonScale.Y - PositionOffsetY;
		float V = (1.0f / float(HeightmapSizeY)) * float(VertexPositionY);

		uint32 HeightmapPixelOffsetY = HeightmapY;
			//HeightmapMirrorY ? (HeightmapSizeY - 1 - HeightmapY) : HeightmapY;

		for (uint32 HeightmapX = 0; HeightmapX < HeightmapSizeX; ++HeightmapX)
		{
			uint32 HeightmapPixelOffsetX = HeightmapX;
				//HeightmapMirrorX ? (HeightmapSizeX - 1 - HeightmapX) : HeightmapX;
			uint32 VertexIndex = HeightmapY * HeightmapSizeX + HeightmapX;

			int32 HeightmapZ = int32(Heightmap[HeightmapPixelOffsetY * HeightmapSizeX + HeightmapPixelOffsetX]);
				// GroundHeight - int32(Heightmap[HeightmapPixelOffsetY * HeightmapSizeX + HeightmapPixelOffsetX]);

			FVector& Vertex = Vertices[VertexIndex];

			Vertex.X = float(HeightmapX) * PolygonScale.X - PositionOffsetX;
			Vertex.Y = VertexPositionY;
			Vertex.Z = float(HeightmapZ) * PolygonScale.Z;

			FVector2D& UV = UV0[VertexIndex];

			UV.X = (1.0f / float(HeightmapSizeX)) * float(Vertex.X);
			UV.Y = V;

			if (HeightmapX && HeightmapY) // build triangles
			{
				uint32 VertexIndex0 = VertexIndex - VerticesPerRow;
				uint32 VertexIndex1 = VertexIndex - 1;
				uint32 VertexIndex2 = VertexIndex;
				uint32 VertexIndex3 = VertexIndex - VerticesPerRow + 1;

				if (bResolutionChanged) // rebuild triangles if resolution was changed
				{
					uint32 TriangleIndex = VertexIndex * 6;
					Triangles[TriangleIndex] = VertexIndex0;
					Triangles[TriangleIndex + 1] = VertexIndex1;
					Triangles[TriangleIndex + 2] = VertexIndex2;
					Triangles[TriangleIndex + 3] = VertexIndex0;
					Triangles[TriangleIndex + 4] = VertexIndex2;
					Triangles[TriangleIndex + 5] = VertexIndex3;
				}

				// TODO: Check.
				Tangents[VertexIndex0] = FProcMeshTangent((Vertices[VertexIndex0] - Vertices[VertexIndex1]).GetSafeNormal(), true);
				Tangents[VertexIndex1] = FProcMeshTangent((Vertices[VertexIndex1] - Vertices[VertexIndex2]).GetSafeNormal(), true);
				Tangents[VertexIndex2] = FProcMeshTangent((Vertices[VertexIndex2] - Vertices[VertexIndex3]).GetSafeNormal(), true);
				Tangents[VertexIndex3] = FProcMeshTangent((Vertices[VertexIndex3] - Vertices[VertexIndex0]).GetSafeNormal(), true);

				// TODO: Check.
				Normals[VertexIndex0] = FVector::CrossProduct(Vertices[VertexIndex0] - Vertices[VertexIndex3], Vertices[VertexIndex0] - Vertices[VertexIndex1]).GetSafeNormal();
				Normals[VertexIndex1] = FVector::CrossProduct(Vertices[VertexIndex1] - Vertices[VertexIndex0], Vertices[VertexIndex1] - Vertices[VertexIndex2]).GetSafeNormal();
				Normals[VertexIndex2] = FVector::CrossProduct(Vertices[VertexIndex2] - Vertices[VertexIndex1], Vertices[VertexIndex2] - Vertices[VertexIndex3]).GetSafeNormal();
				Normals[VertexIndex3] = FVector::CrossProduct(Vertices[VertexIndex3] - Vertices[VertexIndex2], Vertices[VertexIndex3] - Vertices[VertexIndex0]).GetSafeNormal();
			}
		}
	}

	if (!bResolutionChanged) // update procedural mesh section
	{
		this->UpdateMeshSection(0, Vertices, Normals, UV0, VertexColors, Tangents);
	}
	else // create procedural mesh section
	{
		this->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);
		// this->SetMaterial(0, LandscapeMaterial);
	}

	UpdateLandscapeNavigationGrid();
}

void UDyanmicGridMeshComponent::UpdateLandscapeNavigationGrid()
{
	/*
	if (UNavigationSystem::ShouldUpdateNavOctreeOnComponentChange())
	{
		// TODO: Check.
		UWorld* World = GetWorld();
		if (World && World->IsGameWorld() && World->GetNetMode() < ENetMode::NM_Client)
		{
			UNavigationSystem::UpdateComponentInNavOctree(*this);
		}
	}
	*/
}
