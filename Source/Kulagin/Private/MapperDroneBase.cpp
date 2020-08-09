// Fill out your copyright notice in the Description page of Project Settings.

#include "MapperDroneBase.h"

#include "KulaginStatics.h"

AMapperDroneBase::AMapperDroneBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMapperDroneBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMapperDroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AMapperDroneBase::DropMovingActor_Implementation()
{
	//
	return false;
}

bool AMapperDroneBase::RiseMovingActor_Implementation()
{
	//
	return false;
}

bool AMapperDroneBase::SetPaused_Implementation(bool bIsPausedIn)
{
	//

	return IsPaused();
}

bool AMapperDroneBase::IsPaused_Implementation()
{
	//

	return true;
}

bool AMapperDroneBase::SetMovingActorLocation_Implementation(const FVector& Loc, const bool bTeleport)
{
	return SetActorLocation(Loc, false, nullptr, TeleportFlagToEnum(bTeleport));
}

bool AMapperDroneBase::SetMovingActorRotation_Implementation(const FRotator& Rot, const bool bTeleport)
{
	return SetActorRotation(Rot, TeleportFlagToEnum(bTeleport));
}