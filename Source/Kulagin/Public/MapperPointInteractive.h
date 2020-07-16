// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapperPointBase.h"
#include "KulaginTypes.h"

#include "MapperPointInteractive.generated.h"

class AMapperContainerBase;

UCLASS(Abstract, BlueprintType)
class KULAGIN_API AMapperPointInteractive : public AMapperPointBase
{
	GENERATED_BODY()

public:
	
	AMapperPointInteractive();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdatePlane();
	virtual bool UpdatePlane_Implementation();

	/* Update point data from actor properties in game world */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateDataFromActor();
	virtual bool UpdateDataFromActor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetPointIndex(int32 IndexIn);
	virtual bool SetPointIndex_Implementation(int32 IndexIn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateProjection();
	virtual bool UpdateProjection_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetProjectionEnabled(bool bEnabledIn);
	virtual bool SetProjectionEnabled_Implementation(bool bEnabledIn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateFirstLast();
	virtual bool UpdateFirstLast_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateProjectionComponents(USceneComponent* Root, USceneComponent* Decal);
	virtual bool UpdateProjectionComponents_Implementation(USceneComponent* Root, USceneComponent* Decal);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetFirst();
	virtual bool SetFirst_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetLast();
	virtual bool SetLast_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool IsFirst();
	virtual bool IsFirst_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool IsLast();
	virtual bool IsLast_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool SetNormal();
	virtual bool SetNormal_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool ShowArrow();
	virtual bool ShowArrow_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool HideArrow();
	virtual bool HideArrow_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	float GetWaitTime();
	virtual float GetWaitTime_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	float GetEnterTime();
	virtual float GetEnterTime_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	float GetLeaveTime();
	virtual float GetLeaveTime_Implementation();

	UFUNCTION(BlueprintPure, Category = "Mapper", meta = (DeterminesOutputType = "PointClass", DynamicOutputParam = "Previous"))
	void GetPreviousOfClass(AMapperPointBase*& Previous, TSubclassOf<AMapperPointBase> PointClass);

	UFUNCTION(BlueprintPure, Category = "Mapper", meta = (DeterminesOutputType = "PointClass", DynamicOutputParam = "Next"))
	void GetNextOfClass(AMapperPointBase*& Next, TSubclassOf<AMapperPointBase> PointClass);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	AMapperPointBase* GetPrevious();
	virtual AMapperPointBase* GetPrevious_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Mapper")
	AMapperPointBase* GetNext();
	virtual AMapperPointBase* GetNext_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateSpeed(float NewTargetTimeIn);
	virtual bool UpdateSpeed_Implementation(float NewTargetTimeIn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateCrossRadius();
	virtual bool UpdateCrossRadius_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateCrossCylinder();
	virtual bool UpdateCrossCylinder_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdateCrossVisibility();
	virtual bool UpdateCrossVisibility_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	bool UpdatePathVisibility();
	virtual bool UpdatePathVisibility_Implementation();

	// 

	UPROPERTY(BlueprintReadWrite)
	AMapperPointBase* TargetPoint;

	UPROPERTY(BlueprintReadWrite)
	FVector PrevLocation;

	UPROPERTY(BlueprintReadWrite)
	FVector TargetPrevLocation;

	UPROPERTY(BlueprintReadWrite)
	AMapperContainerBase* ParentContainer;

	UPROPERTY(BlueprintReadWrite)
	int32 PointIndex;

	UPROPERTY(BlueprintReadWrite)
	float EnterTime;

	UPROPERTY(BlueprintReadWrite)
	bool StaticPointStruct;

	UPROPERTY(BlueprintReadWrite)
	bool bIsProjectionEnabled;

	UPROPERTY(BlueprintReadWrite)
	bool bIsFirstLast;

	UPROPERTY(BlueprintReadWrite)
	UTextureRenderTarget2D* CrossRenderTarget;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* CrossMaterial;
};