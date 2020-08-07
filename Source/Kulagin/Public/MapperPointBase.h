// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KulaginTypes.h"

#include "MapperPointBase.generated.h"

UCLASS(Abstract, BlueprintType)
class KULAGIN_API AMapperPointBase : public AActor
{
	GENERATED_BODY()

public:
	
	AMapperPointBase();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetMaterial(UMaterialInterface* NewMaterial);
	virtual bool SetMaterial_Implementation(UMaterialInterface* NewMaterial);

	UFUNCTION(BlueprintCallable, Category = "Mapper")
	void SetDangerZoneMask(UTextureRenderTarget2D* RenderTarget, float StartTime, float EndTime, const TArray<FDangerZone> &Zones);

	UFUNCTION(BlueprintCallable, Category = "Mapper")
	void DrawDangerZoneMask(UTextureRenderTarget2D* RenderTarget, float StartPercent, float EndPercent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetNextPoint(AMapperPointBase* NextPointIn);
	virtual bool SetNextPoint_Implementation(AMapperPointBase* NextPointIn);

	/* Update actor properties in game world from point data */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateActorFromData();
	virtual bool UpdateActorFromData_Implementation();

	UPROPERTY(BlueprintReadWrite)
	float StartAlt;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetDataFromList(const FMissionPointList& List, const int32 Index);
	virtual bool SetDataFromList_Implementation(const FMissionPointList& List, const int32 Index);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetDataFromPoint(AMapperPointBase* Point);
	virtual bool SetDataFromPoint_Implementation(AMapperPointBase* Point);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	float GetSpeedToPoint(const FVector& CurrentLocation);
	virtual float GetSpeedToPoint_Implementation(const FVector& CurrentLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	float GetTimeToPoint(const FVector& CurrentLocation);
	virtual float GetTimeToPoint_Implementation(const FVector& CurrentLocation);

};