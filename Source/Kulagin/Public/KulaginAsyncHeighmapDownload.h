// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "KulaginTypes.h"

#include "KulaginAsyncHeighmapDownload.generated.h"

class UTexture2DDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapTileDelegate, UTexture2DDynamic*, Texture);

UCLASS()
class KULAGIN_API UKulaginGetMapTile : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=( BlueprintInternalUseOnly="true" ))
	static UKulaginGetMapTile* GetMapTile(const FMapTileInfo& InfoIn);

public:

	UPROPERTY(BlueprintAssignable)
	FMapTileDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FMapTileDelegate OnFail;

public:

	void Start();

private:
	
	FMapTileInfo Info;

	UFUNCTION()
	void OnFileSuccess(UTexture2DDynamic* Texture);

	UFUNCTION()
	void OnFileFail(UTexture2DDynamic* Texture);

	UFUNCTION()
	void OnWebSuccess(UTexture2DDynamic* Texture);

	UFUNCTION()
	void OnWebFail(UTexture2DDynamic* Texture);
};
