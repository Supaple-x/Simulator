// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KulaginTypes.h"

#include "MapperContainerBase.generated.h"

class AMapperPointBase;
class AMapperDroneBase;

UCLASS(Abstract, BlueprintType)
class KULAGIN_API AMapperContainerBase : public AActor
{
	GENERATED_BODY()

public:
	
	AMapperContainerBase();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPointContainerType ContainerType;

	UPROPERTY(BlueprintReadWrite)
	bool bIsPathVisible = true;

	UPROPERTY(BlueprintReadWrite)
	bool bIsPaused;

	UPROPERTY(BlueprintReadWrite)
	TArray<AMapperPointBase*> PointActors;
	
	UPROPERTY(BlueprintReadWrite)
	AMapperDroneBase* DroneActor;

	UPROPERTY(BlueprintReadWrite)
	FBinaryFilePath FilePath;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* TimelineWidget;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* PathMaterialInstance;

	UPROPERTY(BlueprintReadWrite)
	TArray<FDangerZone> DangerZones;

	/* In real life sentimeters */
	UPROPERTY(BlueprintReadWrite)
	float DangerZoneRadius = 300.f;

	UFUNCTION(BlueprintPure, Category = "Mapper")
	float GetDangerZoneRadiusUU() const;

	//

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateDangerZones();
	virtual bool UpdateDangerZones_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	float GetTotalTime() const;
	virtual float GetTotalTime_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetPointsMaterial();
	virtual bool SetPointsMaterial_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetPointsColor(FLinearColor NewColor);
	virtual bool SetPointsColor_Implementation(FLinearColor NewColor);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetMovingActor(float NewTime);
	virtual bool SetMovingActor_Implementation(float NewTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	FVector GetLocationAtTime(float TimeIn);
	virtual FVector GetLocationAtTime_Implementation(float TimeIn);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool StartMoving();
	virtual bool StartMoving_Implementation();
	
	/* 
	 * Return Timeline Is Highlighted
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetTimelineIsHighlighted(bool bNewIsHighlighted);
	virtual bool SetTimelineIsHighlighted_Implementation(bool bNewIsHighlighted);
	
	/* 
	 * Return Track Is Highlighted
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetWorldTrackIsHighlighted(bool bNewIsHighlighted);
	virtual bool SetWorldTrackIsHighlighted_Implementation(bool bNewIsHighlighted);
	
	/* 
	 * Return path visibility
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool TogglePathVisibility();
	virtual bool TogglePathVisibility_Implementation();
	
	/* 
	 * Return path visibility
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetPathVisibility(bool bNewIsVisible);
	virtual bool SetPathVisibility_Implementation(bool bNewIsVisible);
	
	/* 
	 * Return is paused
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool TogglePaused();
	virtual bool TogglePaused_Implementation();
	
	/* 
	 * Return is paused
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetPaused(bool bNewIsPaused);
	virtual bool SetPaused_Implementation(bool bNewIsPaused);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool RemovePath();
	virtual bool RemovePath_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	TArray<AActor*> GetActorsIgnoreCrossing() const;
	virtual TArray<AActor*> GetActorsIgnoreCrossing_Implementation() const;

};