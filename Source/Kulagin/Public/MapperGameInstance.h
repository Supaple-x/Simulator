// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CPP_GameInstanceBase.h"

#include "MapperGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class KULAGIN_API UMapperGameInstance : public UCPP_GameInstanceBase
{
	GENERATED_BODY()
	
public:

	UMapperGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init() override;

	virtual void Shutdown() override;

	UPROPERTY(Category = "Mapper", BlueprintReadWrite, EditDefaultsOnly)
	FName LevelName;

	UPROPERTY(Category = "Mapper", BlueprintReadWrite, EditDefaultsOnly)
	FName DroneName;

	/** GameMode is optional parameter, may be None */
	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mapper")
	//FName GetActualLevelName();
	//virtual FName GetActualLevelName_Implementation();

};
