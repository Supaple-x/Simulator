// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CPP_GameInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class AIRSIM_API UCPP_GameInstanceBase : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterType")
	bool bCharacterIsDrone = true;
};
