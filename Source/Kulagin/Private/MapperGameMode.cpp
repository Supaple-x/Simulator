// Fill out your copyright notice in the Description page of Project Settings.

#include "MapperGameMode.h"

#include "MapperContainerBase.h"
#include "MapperTileComponent.h"

#include "KulaginStatics.h"


AMapperGameMode::AMapperGameMode()
{
	//
}

AMapperGameMode::~AMapperGameMode()
{
	MinZoomImages.Empty();
	MaxZoomImages.Empty();
}

void AMapperGameMode::PostLogin(APlayerController* NewPlayer)
{
	MinZoomImages.Empty();
	MaxZoomImages.Empty();

	Super::PostLogin(NewPlayer);
	
	UE_LOG(LogTemp, Warning, TEXT("MapperGameMode: PostLogin"));
}

void AMapperGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	UE_LOG(LogTemp, Warning, TEXT("MapperGameMode: Logout"));


}

bool AMapperGameMode::UpdateAllDangerZones_Implementation()
{
	for (AMapperContainerBase* CurrentContainer : PointContainers)
	{
		if (CurrentContainer)
			CurrentContainer->UpdateDangerZones();
	};
	return true;
}

AActor* AMapperGameMode::GetSceneActor_Implementation()
{
	return nullptr;
}

TArray<AMapperContainerBase*> AMapperGameMode::FindDangerZonesAtTime_Implementation(const float InTime)
{
	TArray<AMapperContainerBase*> Temp;
	for (AMapperContainerBase* CurrentContainer : PointContainers)
	{
		if (CurrentContainer && CurrentContainer->IsTimeInDangerZone(InTime))
			Temp.Add(CurrentContainer);
	}
	return Temp;
}

float AMapperGameMode::GetHeighmapMinZ()
{
	AActor* SceneActor = GetSceneActor();
	return SceneActor != nullptr ? SceneActor->GetActorLocation().Z * -1.f : 0.f;
}

void AMapperGameMode::UpdateHeighmapMinZ(const float MinZ)
{
	if (MinZ >= HeighmapMinZ)
		return;
	
	HeighmapMinZ = MinZ;

	AActor* SceneActor = GetSceneActor();

	if (SceneActor == nullptr)
		return;

	FVector Loc = SceneActor->GetActorLocation();
	Loc.Z = HeighmapMinZ * -1.f;

	SceneActor->SetActorLocation(Loc);
}

void AMapperGameMode::ResetHeighmapMinZ()
{
	HeighmapMinZ = 999999.f;

	AActor* SceneActor = GetSceneActor();

	if (SceneActor == nullptr)
		return;

	FVector Loc = SceneActor->GetActorLocation();
	Loc.Z = 0.f;
	SceneActor->SetActorLocation(Loc);
}

void AMapperGameMode::SetAutoHeighmapEnabled(const bool bEnabledIn)
{
	UE_LOG(LogTemp, Warning, TEXT("MapperGameMode: SetAutoHeighmapEnabled"));

	AActor* Scene = GetSceneActor();
	if (Scene == nullptr) return;

	bAutoHeighmapEnabled = bEnabledIn;

	if (bAutoHeighmapEnabled == false) return;

	TSet<UActorComponent*> Components = Scene->GetComponents();

	for (UActorComponent* Current : Components)
	{
		UMapperTileComponent* Tile = Cast<UMapperTileComponent>(Current);
		if (Tile == nullptr) continue;

		Tile->SetComponentTickEnabled(true);
		Tile->UpdateTile();
	}

	UE_LOG(LogTemp, Warning, TEXT("MapperGameMode: SetAutoHeighmapEnabled: Done for %i components"), Components.Num());
}

void AMapperGameMode::UpdateCurrentTilesHeighmap()
{
	UE_LOG(LogTemp, Warning, TEXT("MapperGameMode: SetAutoHeighmapEnabled"));

	AActor* Scene = GetSceneActor();
	if (Scene == nullptr) return;

	TSet<UActorComponent*> Components = Scene->GetComponents();

	for (UActorComponent* Current : Components)
	{
		UMapperTileComponent* Tile = Cast<UMapperTileComponent>(Current);
		if (Tile == nullptr || Tile->bForceHeighmap) continue;

		Tile->bForceHeighmap = true;
		Tile->SetComponentTickEnabled(true);
		Tile->UpdateTile();
	}

	UE_LOG(LogTemp, Warning, TEXT("MapperGameMode: SetAutoHeighmapEnabled: Done for %i components"), Components.Num());
}