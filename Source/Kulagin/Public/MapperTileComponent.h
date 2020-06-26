// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "KulaginTypes.h"

#include "MapperTileComponent.generated.h"

class UTexture2DDynamic;

UCLASS(editinlinenew, meta = (BlueprintSpawnableComponent))
class KULAGIN_API UMapperTileComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
	
public:

	// Procedural stuff begin

	UPROPERTY()
	TArray<FVector> Vertices;

	UPROPERTY()
	TArray<FVector> Normals;

	UPROPERTY()
	TArray<int32> Triangles;

	UPROPERTY()
	TArray<FVector2D> UVs;

	UPROPERTY()
	TArray<FLinearColor> VertexColors;

	UPROPERTY()
	TArray<FProcMeshTangent> Tangents;



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Width = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Spacing = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool GenerateMesh;

	void GenerateVertices();
	void GenerateTriangles();

	void UpdateHeighmapVertices(UMapperHeighmap* Heighmap, const int32 IndexX, const int32 IndexY, const int32 IndexCount);

	// Procedural stuff end 

	UMapperTileComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void UpdateTile();

	UFUNCTION(BlueprintCallable)
	void RoundTile();

	UFUNCTION(BlueprintPure)
	bool IsInPlayerView() const;

	UFUNCTION(BlueprintPure)
	bool IsInRenderDistance() const;
	
	UPROPERTY(BlueprintReadWrite)
	UMapperTileComponent* LeftTile;
	
	UPROPERTY(BlueprintReadWrite)
	UMapperTileComponent* TopTile;
	
	UPROPERTY(BlueprintReadWrite)
	UMapperTileComponent* RightTile;
	
	UPROPERTY(BlueprintReadWrite)
	UMapperTileComponent* BottomTile;

	// MAX ZOOM

	UFUNCTION(BlueprintCallable)
	void GetMapTileMaxZoom();

	UFUNCTION(BlueprintCallable)
	void LoadBaseColorMaxZoom();

	UFUNCTION(BlueprintCallable)
	void LoadHeighmapMaxZoom();

	// MIN ZOOM

	UFUNCTION(BlueprintCallable)
	void GetMapTileMinZoom();

	UFUNCTION(BlueprintCallable)
	void LoadBaseColorMinZoom();

	UFUNCTION(BlueprintCallable)
	void LoadHeighmapMinZoom();

	UFUNCTION(BlueprintCallable)
	void SetBaseColorMinZoom(const FMapTileImage &TileImage);

	UFUNCTION(BlueprintCallable)
	void SetHeighmapMinZoom(const FMapTileImage &TileImage);

	UPROPERTY(BlueprintReadWrite)
	ETileImageState BaseColorState;

	UPROPERTY(BlueprintReadWrite)
	ETileImageState HeighmapState;

	UFUNCTION(BlueprintPure)
	bool IsRounded();

private:

	FMapTileInfo MinZoomInfo;

	FMapTileInfo MaxZoomInfo;

	bool bRounded = false;

	UFUNCTION()
	void OnMapTileMaxZoomSuccess(UTexture2DDynamic* Texture);

	UFUNCTION()
	void OnMapTileMaxZoomFail(UTexture2DDynamic* Texture);

	UFUNCTION()
	void OnMapTileMinZoomSuccess(UTexture2DDynamic* Texture);

	UFUNCTION()
	void OnMapTileMinZoomFail(UTexture2DDynamic* Texture);

	UFUNCTION()
	void OnHeighmapMaxZoomSuccess(UMapperHeighmap* Heighmap);

	UFUNCTION()
	void OnHeighmapMaxZoomFail(UMapperHeighmap* Heighmap);

	UFUNCTION()
	void OnHeighmapMinZoomSuccess(UMapperHeighmap* Heighmap);

	UFUNCTION()
	void OnHeighmapMinZoomFail(UMapperHeighmap* Heighmap);
	
};