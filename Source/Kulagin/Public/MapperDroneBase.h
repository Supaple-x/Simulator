// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KulaginTypes.h"

#include "MapperDroneBase.generated.h"

UCLASS(Abstract, BlueprintType)
class KULAGIN_API AMapperDroneBase : public AActor
{
	GENERATED_BODY()

public:
	
	AMapperDroneBase();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool DropMovingActor();
	virtual bool DropMovingActor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool RiseMovingActor();
	virtual bool RiseMovingActor_Implementation();

	/* Return IsPaused */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetPaused(bool bIsPausedIn);
	virtual bool SetPaused_Implementation(bool bIsPausedIn);

	/* Return IsPaused */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	bool IsPaused();
	virtual bool IsPaused_Implementation();

	/* 
	 * Using for SetMovingActor
	 * Return is succeed 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetMovingActorLocation(const FVector& Loc, const bool bTeleport = false);
	virtual bool SetMovingActorLocation_Implementation(const FVector& Loc, const bool bTeleport = false);

	/*
	 * Using for SetMovingActor
	 * Return is succeed 
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetMovingActorRotation(const FRotator& Rot, const bool bTeleport = false);
	virtual bool SetMovingActorRotation_Implementation(const FRotator& Rot, const bool bTeleport = false);

};