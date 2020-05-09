// Fill out your copyright notice in the Description page of Project Settings.

#include "MapperGameMode.h"

#include "MapperContainerBase.h"

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
