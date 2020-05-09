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
};
