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

};