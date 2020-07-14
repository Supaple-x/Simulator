// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"

#include "KulaginTypes.h"

#include "MapperGameMode.generated.h"

class AMapperContainerBase;

/**
 * 
 */
UCLASS()
class KULAGIN_API AMapperGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	AMapperGameMode();

	virtual ~AMapperGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	UPROPERTY(BlueprintReadWrite)
	TArray<AMapperContainerBase*> PointContainers;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* TileMaterial;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* TileMesh;

	/* Google Tiles */
	UPROPERTY(BlueprintReadWrite)
	TMap<FIntPoint, FMapTileImage> MinZoomImages = {};

	/* Google Tiles */
	UPROPERTY(BlueprintReadWrite)
	TSet<FIntPoint> MaxZoomImages = {};

	/* Google Tiles */
	UFUNCTION(BlueprintCallable)
	void SetMaxZoomImages(TSet<FIntPoint> MaxZoomImagesIn) { MaxZoomImages = MaxZoomImagesIn; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateAllDangerZones();
	virtual bool UpdateAllDangerZones_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	AActor* GetSceneActor();
	virtual AActor* GetSceneActor_Implementation();

	/** Return containers with moving actors in any danger zone at given time */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mapper")
	TArray<AMapperContainerBase*> FindDangerZonesAtTime(const float InTime);
	virtual TArray<AMapperContainerBase*> FindDangerZonesAtTime_Implementation(const float InTime);

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	float HeighmapMinZ = 999999.f;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	float HeighmapMaxZ = 0.f;

	UFUNCTION(BlueprintPure, Category = "Mapper")
	float GetHeighmapMinZ();

	UFUNCTION(BlueprintCallable, Category = "Mapper")
	void UpdateHeighmapMinZ(const float MinZ);

	UFUNCTION(BlueprintCallable, Category = "Mapper")
	void ResetHeighmapMinZ();

#if WITH_EDITORONLY_DATA 
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ETileImageState BaseColorStateInit;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ETileImageState HeighmapStateInit;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bEnableDefaultTiles = true;

	/* Min, Max */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FIntPoint DefaultTilesCount = FIntPoint(8, 16);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bEnableRoundTiles = true;

#endif

	bool bAutoHeighmapEnabled = false;

	UFUNCTION(BlueprintPure, Category = "Mapper")
	bool IsAutoHeighmapEnabled() const { return bAutoHeighmapEnabled; };

	UFUNCTION(BlueprintCallable, Category = "Mapper")
	void SetAutoHeighmapEnabled(const bool bEnabledIn);

	UFUNCTION(BlueprintCallable, Category = "Mapper")
	void UpdateCurrentTilesHeighmap();
};
