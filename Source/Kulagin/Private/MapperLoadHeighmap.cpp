// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MapperLoadHeighmap.h"
#include "KulaginStatics.h"
//#include "Modules/ModuleManager.h"
//#include "Engine/Texture2D.h"
//#include "Engine/Texture2DDynamic.h"
//#include "IImageWrapper.h"
//#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"

#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "JsonObjectConverter.h"
#include "ImageUtils.h"

#include "UMG/Public/Blueprint/AsyncTaskDownloadImage.h"

UMapperLoadHeighmap::UMapperLoadHeighmap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AddToRoot();
}

UMapperLoadHeighmap* UMapperLoadHeighmap::LoadHeighmapTile(const FMapTileInfo& InfoIn)
{
	UMapperLoadHeighmap* GetHeighmapTask = NewObject<UMapperLoadHeighmap>();
	GetHeighmapTask->Corners.Add(InfoIn.TopLeft);
	GetHeighmapTask->Corners.Add(InfoIn.TopRight);
	GetHeighmapTask->Corners.Add(InfoIn.BottomLeft);
	GetHeighmapTask->Corners.Add(InfoIn.BottomRight);
	GetHeighmapTask->FilePath = InfoIn.GetFilename();
	GetHeighmapTask->Start();

	return GetHeighmapTask;
}

UMapperLoadHeighmap* UMapperLoadHeighmap::LoadHeighmap(const FString Filename, const FIntPoint Size, const TArray<USceneComponent*> CornersComponents)
{
	UMapperLoadHeighmap* GetHeighmapTask = NewObject<UMapperLoadHeighmap>();
	for (int32 i = 0; i <= 3; i++)
	{
		if (CornersComponents.IsValidIndex(i) && CornersComponents[i])
			GetHeighmapTask->Corners.Add(UKulaginStatics::UEToLatLon(CornersComponents[i]->GetComponentLocation()));
		else
			GetHeighmapTask->Corners.Add(FLatLon());
	}
	GetHeighmapTask->Start();

	return GetHeighmapTask;
}

void UMapperLoadHeighmap::Start()
{
	UAsyncTaskDownloadImage* DownloadTask = NewObject<UAsyncTaskDownloadImage>();
	DownloadTask->OnSuccess.AddDynamic(this, &UMapperLoadHeighmap::OnFileSuccess);
	DownloadTask->OnFail.AddDynamic(this, &UMapperLoadHeighmap::OnFileFail);
	DownloadTask->Start(L"path to file in [Saved]/MapTileCache or etc");
}

void UMapperLoadHeighmap::OnFileSuccess(UTexture2DDynamic* Texture)
{
	OnSuccess.Broadcast(nullptr);
	RemoveFromRoot();
}

void UMapperLoadHeighmap::OnFileFail(UTexture2DDynamic* Texture)
{
	const FLatLon& SouthWest = Corners[2];
	const FLatLon& NorthEast = Corners[1];

	// https://api.airmap.com/elevation/v1/ele/carpet?points=[sw, ne]
	// https://api.airmap.com/elevation/v1/ele/carpet?points=49.9760329253738,14.128741875966284,49.97493584462879,14.130635515530571

	const FString South = UKulaginStatics::GetValidStringFromDouble(SouthWest.Lat);
	const FString West = UKulaginStatics::GetValidStringFromDouble(SouthWest.Lon);
	const FString North = UKulaginStatics::GetValidStringFromDouble(NorthEast.Lat);
	const FString East = UKulaginStatics::GetValidStringFromDouble(NorthEast.Lon);

	const FString URL = L"https://api.airmap.com/elevation/v1/ele/carpet?points=" + South + L"," + West + L"," + North + L"," + East;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperLoadHeighmap: OnFileFail: URL = \n%s"), *URL);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &UMapperLoadHeighmap::OnRequestComplete);
	Request->SetURL(URL);
	Request->SetVerb(L"GET");

	Request->SetHeader(L"Content-Type", L"application/json; charset=utf-8");

	Request->ProcessRequest();
}

void UMapperLoadHeighmap::OnWebSuccess(UMapperHeighmap* Heighmap)
{
	// TODO Save texture to file in [Saved]/MapTileCache or etc
	OnSuccess.Broadcast(Heighmap);
	RemoveFromRoot();
}

void UMapperLoadHeighmap::OnWebFail(UMapperHeighmap* Heighmap)
{
	OnFail.Broadcast(nullptr);
	RemoveFromRoot();
}

void UMapperLoadHeighmap::OnRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful == false || Response.IsValid() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperLoadHeighmap: OnRequestComplete: FAILED"));

		OnWebFail(nullptr);
		return;
	}

	FString JsonString = Response->GetContentAsString();

	UE_LOG(LogTemp, Error, TEXT("Kulagin: MapperLoadHeighmap: OnRequestComplete url = \n-----------------------------\n%s\n-----------------------------\n"), Request.IsValid() ? *Request->GetURL() : *FString("FAILED"));

	UE_LOG(LogTemp, Error, TEXT("Kulagin: MapperLoadHeighmap: OnRequestComplete = \n-----------------------------\n%s\n-----------------------------\n"), *JsonString);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (FJsonSerializer::Deserialize(Reader, JsonObject) == false)
	{
		OnWebFail(nullptr);
		return;
	}

	TSharedPtr<FJsonObject> Data = JsonObject->GetObjectField(L"data");
	TArray<TSharedPtr<FJsonValue>> BaseCarpet = Data->GetArrayField(L"carpet");

	TSharedPtr<FJsonObject> Stats = Data->GetObjectField(L"stats");

	const double MinDouble = Stats->GetNumberField(L"min");
	const double MaxDouble = Stats->GetNumberField(L"max");
	const int32 Min = MinDouble <= 100 ? MinDouble - 5. : MinDouble * 0.95;
	const int32 Max = MaxDouble <= 100 ? MaxDouble + 5. : MaxDouble * 1.05;
	const double Delta = (Max - Min) <= 0 ? 255. : Max - Min;
	const double HeighMP = 255. / Delta;

	UE_LOG(LogTemp, Error, TEXT("Kulagin: MapperLoadHeighmap: OnRequestComplete: HeighMP: %f, Min: %i, Max: %i"), HeighMP, Min, Max);

	// meter = R + G

	const int32 SizeMP = 1;

	if (BaseCarpet.IsValidIndex(0) == false || BaseCarpet[0].IsValid() == false)
	{
		OnWebFail(nullptr);
		return;
	}
	const TArray<TSharedPtr<FJsonValue>> FirstLineValues = BaseCarpet[0]->AsArray();
	const int32 SizeX = FirstLineValues.Num() * SizeMP;
	const int32 SizeY = BaseCarpet.Num() * SizeMP;
	if (SizeX <= 0 || SizeY <= 0)
	{
		OnWebFail(nullptr);
		return;
	}

	UMapperHeighmap* Heighmap = NewObject<UMapperHeighmap>();
	Heighmap->InitData(SizeX, SizeY);

	TArray<int32> &ImageData = Heighmap->Data;
	int32 &MinHeigh = Heighmap->MinHeigh;
	MinHeigh = 99999;

	UE_LOG(LogTemp, Error, TEXT("Kulagin: MapperLoadHeighmap: OnRequestComplete: SizeX = %i, SizeY = %i, ImageData.Num = %i"), SizeX, SizeY, ImageData.Num());

	int32 IndexY = SizeY - 1;
	for (TSharedPtr<FJsonValue>& CarpetLine : BaseCarpet)
	{
		if (CarpetLine.IsValid() == false) continue;
		TArray<TSharedPtr<FJsonValue>> LineValues = CarpetLine->AsArray();
		int32 IndexX = 0;
		for (TSharedPtr<FJsonValue>& CarpetValue : LineValues)
		{
			if (CarpetValue.IsValid() == false) continue;
			const int32 Heigh = CarpetValue->AsNumber();
			if (Heigh < MinHeigh) MinHeigh = Heigh;
			//const uint8 ColorR = FMath::Clamp(Heigh, 0, 255);
			//const uint8 ColorR = FMath::Clamp<int32>(double(Heigh - Min) * HeighMP, 0, 255);
			//const uint8 ColorG = FMath::Clamp(Heigh - 255, 0, 255);
			const int32 CurrentColor = Heigh;
			for (int32 i = 0; i < SizeMP; i++)
				for (int32 j = 0; j < SizeMP; j++)
				{
					//UE_LOG(LogTemp, Error, TEXT("Kulagin: MapperLoadHeighmap: OnRequestComplete: IndexY = %i, i = %i, IndexX = %i, j = %i"), IndexY, i, IndexX, j);
					ImageData[(IndexY * SizeMP + i) * SizeX + (IndexX * SizeMP + j)] = CurrentColor;
				}
			//UE_LOG(LogTemp, Error, TEXT("Kulagin: MapperLoadHeighmap: OnRequestComplete: Heigh: %i"), Heigh);
			IndexX++;
		}
		IndexY--;
	}

	FCreateTexture2DParameters Params;
	Params.bDeferCompression = false;
	Params.bSRGB = false;
	Params.bUseAlpha = false;
	Params.CompressionSettings = TextureCompressionSettings::TC_Default;

	static int32 HeighmapNumber = 0;
	const FString TextureName = L"Heighmap_" + FilePath + L"_Num_" + FString::FromInt(HeighmapNumber++);

	//UTexture2D* Heighmap = FImageUtils::CreateTexture2D(SizeX, SizeY, ImageData, this, TextureName, RF_NoFlags, Params);

	OnWebSuccess(Heighmap);
}


