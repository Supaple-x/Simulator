// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "KulaginTypes.h"

#include "MapperLoadHeighmap.generated.h"

class UTexture2DDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeighmapDelegate, UTexture*, Texture);

UCLASS()
class KULAGIN_API UMapperLoadHeighmap : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=( BlueprintInternalUseOnly="true" ))
	static UMapperLoadHeighmap* LoadHeighmapTile(const FMapTileInfo& InfoIn);
	
	/*
	 * Image corners
	 * 0 - Top Left, 1 - Top Right, 2 - Bottom Left, 3 - Bottom Right
	 */
	UFUNCTION(BlueprintCallable, meta=( BlueprintInternalUseOnly="true" ))
	static UMapperLoadHeighmap* LoadHeighmap(const FString Filename, const FIntPoint Size, const TArray<USceneComponent*> CornersComponents);

public:

	UPROPERTY(BlueprintAssignable)
	FHeighmapDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FHeighmapDelegate OnFail;

public:

	void Start();

private:
	
	FString FilePath;

	/*
	 * Lat Lon corners
	 * 0 - Top Left, 1 - Top Right, 2 - Bottom Left, 3 - Bottom Right
	 */
	TArray<FLatLon> Corners;

	UFUNCTION()
	void OnFileSuccess(UTexture2DDynamic* Texture);

	UFUNCTION()
	void OnFileFail(UTexture2DDynamic* Texture);

	UFUNCTION()
	void OnWebSuccess(UTexture* Texture);

	UFUNCTION()
	void OnWebFail(UTexture* Texture);

	void OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
