// Fill out your copyright notice in the Description page of Project Settings.

#include "MapperTileComponent.h"
#include "Kismet/GameplayStatics.h"
#include "KismetProceduralMeshLibrary.h"
#include "KulaginStatics.h"
#include "MapperGameMode.h"
#include "KulaginAsyncHeighmapDownload.h"
#include "MapperLoadHeighmap.h"
#include "Kulagin.h"

UMapperTileComponent::UMapperTileComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 0.25f;
}

// Procedural stuff begin

void UMapperTileComponent::GenerateVertices()
{
	float UVSpacing = 1.0f / FMath::Max(Height - 1, Width - 1);

	AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this);
	const float MinZ = GM ? GM->GetHeighmapMinZ() : 0.f;

	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			Vertices.Add(FVector(-50.f + x * Spacing, -50.f + y * Spacing, MinZ));
			Normals.Add(FVector(0.0f, 0.0f, 1.0f));
			UVs.Add(FVector2D(x * UVSpacing, y * UVSpacing));
			//VertexColors.Add((x == 2 && y == 2) ? FLinearColor(1.0f, 0.0f, 0.0f, 0.0f) : FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			VertexColors.Add(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
			Tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
		}
	}
}

void UMapperTileComponent::GenerateTriangles()
{
	for (int32 y = 0; y < Height - 1; y++)
	{
		for (int32 x = 0; x < Width - 1; x++)
		{
			Triangles.Add(x + (y * Width));					//current vertex
			Triangles.Add(x + (y * Width) + Width);			//current vertex + row
			Triangles.Add(x + (y * Width) + Width + 1);		//current vertex + row + one right

			Triangles.Add(x + (y * Width));					//current vertex
			Triangles.Add(x + (y * Width) + Width + 1);		//current vertex + row + one right
			Triangles.Add(x + (y * Width) + 1);				//current vertex + one right
		}
	}
}

void UMapperTileComponent::UpdateHeighmapVertices(UMapperHeighmap* Heighmap, const int32 IndexX, const int32 IndexY, const int32 IndexCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: UpdateHeighmapVertices"));

	if (Heighmap == nullptr)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: UpdateHeighmapVertices: Heighmap VALID"));

	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			const float PercentageY = float(y) / float(Height - 1);
			const float PercentageX = float(x) / float(Width - 1);

			const int32 HeighRealLife = Heighmap->GetHeigh(PercentageX, PercentageY, IndexX, IndexY, IndexCount);
			const float HeighUU = UKulaginStatics::LifeToUU(HeighRealLife);

			Vertices[y * Width + x].Z = HeighUU;

			//UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: UpdateHeighmapVertices: HeighUU = %f"), HeighUU);
		}
	}

	const float MinHeighUU = UKulaginStatics::LifeToUU(Heighmap->MinHeigh);

	CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
	if (AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this))
	{
		GM->UpdateHeighmapMinZ(MinHeighUU);
	}
}

// Procedural stuff end

void UMapperTileComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: BeginPlay"));

	const FLatLon LatLon = UKulaginStatics::UEToLatLon(GetComponentLocation());
	MaxZoomInfo = FMapTileInfo(LatLon, UKulaginStatics::MaxZoom);
	MinZoomInfo = FMapTileInfo(LatLon, UKulaginStatics::MinZoom);

	if (AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this))
	{
		//if (GM->TileMesh)
		//	SetStaticMesh(GM->TileMesh);

		if (GM->MinZoomImages.Contains(MinZoomInfo.GetGoogleTile()) == false)
			GM->MinZoomImages.Add(MinZoomInfo.GetGoogleTile(), FMapTileImage(MinZoomInfo));

		GenerateVertices();
		GenerateTriangles();
		CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);

		if (GM->TileMaterial)
			SetMaterial(0, UMaterialInstanceDynamic::Create(GM->TileMaterial, nullptr));

#if WITH_EDITOR
		BaseColorState = GM->BaseColorStateInit;
		HeighmapState = GM->HeighmapStateInit;
#endif

		if (GM->IsAutoHeighmapEnabled()) GetHeighmapMinZoom();
	}

	GetBaseColorMinZoom();

	//GetMapTileMaxZoom();
}

void UMapperTileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: Tick"));

	UpdateTile();
}

void UMapperTileComponent::UpdateTile()
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: UpdateTile: BaseColorState = %i, HeighmapState = %i"), int32(BaseColorState), int32(HeighmapState));

	AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this);
	const bool bAutoUpdate = bForceHeighmap || (GM ? GM->IsAutoHeighmapEnabled() : false);

	if (BaseColorState == ETileImageState::TIS_None)
	{
		GetBaseColorMinZoom();
	}
	if (bAutoUpdate && HeighmapState == ETileImageState::TIS_None)
	{
		GetHeighmapMinZoom();
	}
	if (IsRounded() == false && IsInRenderDistance())
	{
		RoundTile();
	}
	if (BaseColorState > ETileImageState::TIS_None && (bAutoUpdate == false || HeighmapState > ETileImageState::TIS_None) && IsRounded())
	{
		SetComponentTickEnabled(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: UpdateTile: BaseColorState = %i, HeighmapState = %i, IsRounded = %s, AutoUpdate = %s, Tick = %s"),
		int32(BaseColorState), int32(HeighmapState), BToC(IsRounded()), BToC(bAutoUpdate), BToC(IsComponentTickEnabled()));
}

bool UMapperTileComponent::IsRounded()
{ 
	if (bRounded) return true;

	AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this);

#if WITH_EDITOR
	if (GM && GM->bEnableRoundTiles == false) return true;
#endif

	if (GM == nullptr) return false;

	TArray<FIntPoint> NearTiles = { FIntPoint(-1,0),FIntPoint(0,-1) ,FIntPoint(1,0) ,FIntPoint(0,1) };
	for (FIntPoint CurrentNearTile : NearTiles)
	{
		if (GM->MaxZoomImages.Contains(MaxZoomInfo.GetGoogleTile() + CurrentNearTile) == false) return false;
	};

	bRounded = true;
	return true;
}

void UMapperTileComponent::RoundTile()
{
#if WITH_EDITOR
	AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this);
	if (GM && GM->bEnableRoundTiles == false) return;
#endif
	TArray<UMapperTileComponent**> TileReferences;// = { LeftTile, TopTile, RightTile, BottomTile };
	TileReferences.Add(&LeftTile);
	TileReferences.Add(&TopTile);
	TileReferences.Add(&RightTile);
	TileReferences.Add(&BottomTile);

	TArray<FVector> TileLocations = { FVector(-100.f,0.f,0.f), FVector(0.f,-100.f,0.f), FVector(100.f,0.f,0.f), FVector(0.f,100.f,0.f) };

	TArray<FIntPoint> NearTiles = { FIntPoint(-1,0),FIntPoint(0,-1) ,FIntPoint(1,0) ,FIntPoint(0,1) };

	int32 Index = -1;
	for (UMapperTileComponent** CurrentTile : TileReferences)
	{
		Index++;

		const FIntPoint CurrentTileIndex = MaxZoomInfo.GetGoogleTile() + NearTiles[Index];
		if (UKulaginStatics::GetMapperGameMode(this) == nullptr || UKulaginStatics::GetMapperGameMode(this)->MaxZoomImages.Contains(CurrentTileIndex))
			continue;

		UMapperTileComponent*& CurrentTileRef = *CurrentTile;

		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: RoundTile: Index = %i, CurrentTileRef is %s"), Index, *FString(CurrentTileRef ? L"VALID" : L"FAILED"));

		if (CurrentTileRef)
			continue;

		CurrentTileRef = UKulaginStatics::SpawnMapTile(GetAttachParent(), CurrentTileIndex, RelativeLocation + TileLocations[Index]);
	}
	IsRounded();
}

bool UMapperTileComponent::IsInPlayerView() const
{
	if (const UWorld* World = GetWorld())
	{
		const float RenderTimeThreshold = World->DeltaTimeSeconds + KINDA_SMALL_NUMBER;
		return (World->TimeSince(LastRenderTime) <= RenderTimeThreshold);
	}
	return false;
}

bool UMapperTileComponent::IsInRenderDistance() const
{
	const bool bIsInPlayerView = IsInPlayerView();
	const FVector CameraLoc = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
	const FVector TileLoc = GetComponentLocation();
	// Real life distances in meters
	const float DistXY = FVector::DistXY(CameraLoc, TileLoc) / 100.f * UKulaginStatics::Scale;
	const float DistZ = FMath::Abs(CameraLoc.Z - TileLoc.Z) / 100.f * UKulaginStatics::Scale;
	const float RenderDistXY = FMath::Clamp(DistZ + FMath::Pow(DistZ, 1.2f), 150.f, 500.f);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: IsInRenderDistance: IsRounded() = false, bIsInPlayerView = %s"), *FString(bIsInPlayerView ? L"VALID" : L"FAILED"));
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: IsInRenderDistance: DistXY = %i, DistZ = %i, RenderDistXY = %i"), int32(DistXY), int32(DistZ), int32(RenderDistXY));

	return DistXY < 150.f || (bIsInPlayerView && DistXY <= RenderDistXY);
}

void UMapperTileComponent::GetBaseColorMinZoom()
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: GetBaseColorMinZoom: TIS_None"));

	AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this);
	if (GM == nullptr) return;

	FMapTileImage* TileImage = GM->MinZoomImages.Find(MinZoomInfo.GetGoogleTile());
	if (TileImage == nullptr) return;

	if (TileImage->IsBaseColorValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: GetBaseColorMinZoom: IsBaseColorValid VALID"));

		SetBaseColorMinZoom(*TileImage);
	}
	else if (TileImage->bBaseColorBeginLoad == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: GetBaseColorMinZoom: IsBaseColorValid FAILED"));

		TileImage->bBaseColorBeginLoad = LoadBaseColorMinZoom();
	}
}

void UMapperTileComponent::GetHeighmapMinZoom()
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: GetHeighmapMinZoom: TIS_None"));

	AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this);
	if (GM == nullptr) return;

	FMapTileImage* TileImage = GM->MinZoomImages.Find(MinZoomInfo.GetGoogleTile());
	if (TileImage == nullptr) return;

	if (TileImage->IsHeighmapValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: GetHeighmapMinZoom: IsHeighmapValid VALID"));

		SetHeighmapMinZoom(*TileImage);
	}
	else if (TileImage->bHeighmapBeginLoad == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: GetHeighmapMinZoom: IsHeighmapValid FAILED"));

		TileImage->bHeighmapBeginLoad = LoadHeighmapMinZoom();
	}
}

bool UMapperTileComponent::LoadBaseColorMinZoom()
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: LoadBaseColorMinZoom"));

	if (BaseColorState >= ETileImageState::TIS_MinZoom) return false;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: LoadBaseColorMinZoom: < TIS_MinZoom, loc = %s"), *GetComponentLocation().ToString());

	UKulaginGetMapTile* GetMapTileTask = UKulaginGetMapTile::GetMapTile(MinZoomInfo);
	GetMapTileTask->OnSuccess.AddDynamic(this, &UMapperTileComponent::OnMapTileMinZoomSuccess);
	GetMapTileTask->OnFail.AddDynamic(this, &UMapperTileComponent::OnMapTileMinZoomFail);

	if (BaseColorState < ETileImageState::TIS_MinZoom) BaseColorState = ETileImageState::TIS_MinZoom;

	return true;
}

bool UMapperTileComponent::LoadHeighmapMinZoom()
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: LoadHeighmapMinZoom"));

	if (HeighmapState >= ETileImageState::TIS_MinZoom) return false;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: LoadHeighmapMinZoom: < TIS_MinZoom, loc = %s"), *GetComponentLocation().ToString());

	UMapperLoadHeighmap* GetHeighmapTask = UMapperLoadHeighmap::LoadHeighmapTile(MinZoomInfo);
	GetHeighmapTask->OnSuccess.AddDynamic(this, &UMapperTileComponent::OnHeighmapMinZoomSuccess);
	GetHeighmapTask->OnFail.AddDynamic(this, &UMapperTileComponent::OnHeighmapMinZoomFail);

	if (HeighmapState < ETileImageState::TIS_MinZoom) HeighmapState = ETileImageState::TIS_MinZoom;

	return true;
}

void UMapperTileComponent::SetBaseColorMinZoom(const FMapTileImage &TileImage)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: SetBaseColorMinZoom"));

	if (BaseColorState >= ETileImageState::TIS_MinZoom) return;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: SetBaseColorMinZoom: < TIS_MinZoom"));

	if (TileImage.BaseColor == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: SetBaseColorMinZoom: BaseColor VALID"));

	if (UMaterialInstanceDynamic* CurrentInstance = Cast<UMaterialInstanceDynamic>(GetMaterial(0)))
	{
		int32 IndexX, IndexY;
		UKulaginStatics::GetMinZoomIndex(TileImage.Info, GetComponentLocation(), IndexX, IndexY);
		CurrentInstance->SetTextureParameterValue("Tex", TileImage.BaseColor);
		CurrentInstance->SetScalarParameterValue("Zoom", float(UKulaginStatics::GetZoomMaxIndex()));
		CurrentInstance->SetScalarParameterValue("IndexX", float(IndexX));
		CurrentInstance->SetScalarParameterValue("IndexY", float(IndexY));

		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: SetBaseColorMinZoom: instance VALID, indexX = %i, indexY = %i, loc = %s"), IndexX, IndexY, *GetComponentLocation().ToString());

		BaseColorState = ETileImageState::TIS_MinZoom;
	}
}

void UMapperTileComponent::SetHeighmapMinZoom(const FMapTileImage &TileImage)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: SetHeighmapMinZoom"));

	if (HeighmapState >= ETileImageState::TIS_MinZoom)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: SetHeighmapMinZoom: < TIS_MinZoom"));

	if (TileImage.Heighmap == nullptr)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: SetHeighmapMinZoom: Heighmap VALID"));

	int32 IndexX, IndexY;
	UKulaginStatics::GetMinZoomIndex(TileImage.Info, GetComponentLocation(), IndexX, IndexY);
	const int32 IndexCount = UKulaginStatics::GetZoomMaxIndex();

	UpdateHeighmapVertices(TileImage.Heighmap, IndexX, IndexY, IndexCount);

	HeighmapState = ETileImageState::TIS_MinZoom;

	/*if (UMaterialInstanceDynamic* CurrentInstance = Cast<UMaterialInstanceDynamic>(GetMaterial(0)))
	{
		int32 IndexX, IndexY;
		UKulaginStatics::GetMinZoomIndex(TileImage.Info, GetComponentLocation(), IndexX, IndexY);
		CurrentInstance->SetTextureParameterValue("Heighmap", TileImage.Heighmap);
		CurrentInstance->SetScalarParameterValue("Zoom", float(UKulaginStatics::MinZoom));
		CurrentInstance->SetScalarParameterValue("IndexX", float(IndexX));
		CurrentInstance->SetScalarParameterValue("IndexY", float(IndexY));

		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: SetHeighmapMinZoom: instance VALID, indexX = %i, indexY = %i, loc = %s"), IndexX, IndexY, *GetComponentLocation().ToString());

		HeighmapState = ETileImageState::TIS_MinZoom;
	}*/
}

void UMapperTileComponent::GetMapTileMaxZoom()
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: GetMapTileMaxZoom"));

	if (BaseColorState >= ETileImageState::TIS_MaxZoom)// && HeighmapState >= ETileImageState::TIS_MaxZoom)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: GetMapTileMaxZoom: <= TIS_MaxZoom"));

	LoadBaseColorMaxZoom();
	//LoadHeighmapMaxZoom();

	if (BaseColorState < ETileImageState::TIS_MaxZoom) BaseColorState = ETileImageState::TIS_MaxZoom;
	//if (HeighmapState < ETileImageState::TIS_MaxZoom) HeighmapState = ETileImageState::TIS_MaxZoom;
}

void UMapperTileComponent::LoadBaseColorMaxZoom()
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: LoadBaseColorMaxZoom"));

	UKulaginGetMapTile* GetMapTileTask = UKulaginGetMapTile::GetMapTile(MaxZoomInfo);
	GetMapTileTask->OnSuccess.AddDynamic(this, &UMapperTileComponent::OnMapTileMaxZoomSuccess);
	GetMapTileTask->OnFail.AddDynamic(this, &UMapperTileComponent::OnMapTileMaxZoomFail);
}

void UMapperTileComponent::LoadHeighmapMaxZoom()
{
	return;
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: LoadHeighmapMaxZoom"));

	//GetMapTileMinZoom();

	UMapperLoadHeighmap* GetHeighmapTask = UMapperLoadHeighmap::LoadHeighmapTile(MaxZoomInfo);
	GetHeighmapTask->OnSuccess.AddDynamic(this, &UMapperTileComponent::OnHeighmapMaxZoomSuccess);
	GetHeighmapTask->OnFail.AddDynamic(this, &UMapperTileComponent::OnHeighmapMaxZoomFail);
}

void UMapperTileComponent::OnMapTileMaxZoomSuccess(UTexture2DDynamic* Texture)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMaxZoomSuccess"));

	if (Texture == nullptr)
		OnMapTileMaxZoomFail(nullptr);

	UTexture* Test = Texture;
	FTexturePlatformData** DataPtr = Test->GetRunningPlatformData();
	
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMaxZoomSuccess: size DataPtr = %i, size struct ptr = %i"), sizeof(DataPtr), sizeof(FTexturePlatformData*));
	if (DataPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMaxZoomSuccess: DataPtr VALID"));
		FTexturePlatformData* Data = *DataPtr;
		if (Data)
		{
			UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMaxZoomSuccess: Data VALID, mips = %i"), Data->Mips.Num());
		}
	}
	/*for (int32 i = 0; i < (sizeof(DataPtr) / sizeof(FTexturePlatformData*)); i++)
	{
		FTexturePlatformData* Data = DataPtr[i];
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMaxZoomSuccess: i = %i"), i);
		if (Data == nullptr) continue;
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMaxZoomSuccess: Data VALID, mips = %i"), Data->Mips.Num());
	}*/

	if (UMaterialInstanceDynamic* CurrentInstance = Cast<UMaterialInstanceDynamic>(GetMaterial(0)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMaxZoomSuccess: Instance VALID"));

		CurrentInstance->SetTextureParameterValue("Tex", Texture);
		CurrentInstance->SetScalarParameterValue("Zoom", 1.f);
		CurrentInstance->SetScalarParameterValue("IndexX", 0.f);
		CurrentInstance->SetScalarParameterValue("IndexY", 0.f);
	}
}

void UMapperTileComponent::OnMapTileMaxZoomFail(UTexture2DDynamic* Texture)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMaxZoomFail"));

	LoadBaseColorMaxZoom();
}

void UMapperTileComponent::OnMapTileMinZoomSuccess(UTexture2DDynamic* Texture)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMinZoomSuccess"));

	if (BaseColorState == ETileImageState::TIS_MinZoom) BaseColorState = ETileImageState::TIS_None;

	AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this);
	if (GM == nullptr) return;

	FMapTileImage* TileImage = GM->MinZoomImages.Find(MinZoomInfo.GetGoogleTile());
	if (TileImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMinZoomSuccess: TileImage VALID"));

		TileImage->BaseColor = Texture;
		SetBaseColorMinZoom(*TileImage);
	} 
}

void UMapperTileComponent::OnMapTileMinZoomFail(UTexture2DDynamic* Texture)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnMapTileMinZoomFail"));

	LoadBaseColorMinZoom();
}

void UMapperTileComponent::OnHeighmapMaxZoomSuccess(UMapperHeighmap* Heighmap)
{
	return;
	/*UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnHeighmapMaxZoomSuccess"));

	if (Texture == nullptr)
		OnHeighmapMaxZoomFail(nullptr);

	if (UMaterialInstanceDynamic* CurrentInstance = Cast<UMaterialInstanceDynamic>(GetMaterial(0)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnHeighmapMaxZoomSuccess: Instance VALID"));

		CurrentInstance->SetTextureParameterValue("Heighmap", Texture);
		CurrentInstance->SetScalarParameterValue("Zoom", float(UKulaginStatics::MaxZoom));
		CurrentInstance->SetScalarParameterValue("IndexX", 0.f);
		CurrentInstance->SetScalarParameterValue("IndexY", 0.f);
	}*/
}

void UMapperTileComponent::OnHeighmapMaxZoomFail(UMapperHeighmap* Heighmap)
{
	return;
	/*UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnHeighmapMaxZoomFail"));

	LoadHeighmapMaxZoom();*/
}

void UMapperTileComponent::OnHeighmapMinZoomSuccess(UMapperHeighmap* Heighmap)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnHeighmapMinZoomSuccess"));

	if (HeighmapState == ETileImageState::TIS_MinZoom) HeighmapState = ETileImageState::TIS_None;

	AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(this);
	if (GM == nullptr) return;

	FMapTileImage* TileImage = GM->MinZoomImages.Find(MinZoomInfo.GetGoogleTile());
	if (TileImage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnHeighmapMinZoomSuccess: TileImage VALID"));

		TileImage->Heighmap = Heighmap;
		SetHeighmapMinZoom(*TileImage);
	}
}

void UMapperTileComponent::OnHeighmapMinZoomFail(UMapperHeighmap* Heighmap)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: MapperTileComponent: OnHeighmapMinZoomFail"));

	LoadHeighmapMinZoom();
}