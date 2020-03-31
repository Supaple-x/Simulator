// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "KulaginAsyncHeighmapDownload.h"
//#include "Modules/ModuleManager.h"
//#include "Engine/Texture2D.h"
//#include "Engine/Texture2DDynamic.h"
//#include "IImageWrapper.h"
//#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"

#include "UMG/Public/Blueprint/AsyncTaskDownloadImage.h"

UKulaginGetMapTile::UKulaginGetMapTile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AddToRoot();
}

UKulaginGetMapTile* UKulaginGetMapTile::GetMapTile(const FMapTileInfo& InfoIn)
{
	UKulaginGetMapTile* GetMapTileTask = NewObject<UKulaginGetMapTile>();
	GetMapTileTask->Info = InfoIn;
	GetMapTileTask->Start();

	return GetMapTileTask;
}

void UKulaginGetMapTile::Start()
{
	UAsyncTaskDownloadImage* DownloadTask = NewObject<UAsyncTaskDownloadImage>();
	DownloadTask->OnSuccess.AddDynamic(this, &UKulaginGetMapTile::OnFileSuccess);
	DownloadTask->OnFail.AddDynamic(this, &UKulaginGetMapTile::OnFileFail);
	DownloadTask->Start(L"path to file in [Saved]/MapTileCache or etc");
}

void UKulaginGetMapTile::OnFileSuccess(UTexture2DDynamic* Texture)
{
	OnSuccess.Broadcast(Texture);
	RemoveFromRoot();
}

void UKulaginGetMapTile::OnFileFail(UTexture2DDynamic* Texture)
{
	int32 GoogleX, GoogleY;
	Info.GetGoogleTile(GoogleX, GoogleY);
	FString Path(L"http://mt1.google.com/vt/lyrs=y&x=" + FString::FromInt(GoogleX) + L"&y=" + FString::FromInt(GoogleY) + L"&z=" + FString::FromInt(Info.Zoom));
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetMapTile: OnFileFail: Lat = %f, Lon = %f"), Info.Center.Lat, Info.Center.Lon);
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetMapTile: OnFileFail: Path = %s"), *Path);
	UAsyncTaskDownloadImage* DownloadTask = NewObject<UAsyncTaskDownloadImage>();
	DownloadTask->OnSuccess.AddDynamic(this, &UKulaginGetMapTile::OnWebSuccess);
	DownloadTask->OnFail.AddDynamic(this, &UKulaginGetMapTile::OnWebFail);
	DownloadTask->Start(Path);
}

void UKulaginGetMapTile::OnWebSuccess(UTexture2DDynamic* Texture)
{
	// TODO Save texture to file in [Saved]/MapTileCache or etc
	OnSuccess.Broadcast(Texture);
	RemoveFromRoot();
}

void UKulaginGetMapTile::OnWebFail(UTexture2DDynamic* Texture)
{
	OnFail.Broadcast(nullptr);
	RemoveFromRoot();
}
