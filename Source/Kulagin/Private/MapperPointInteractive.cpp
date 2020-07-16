// Fill out your copyright notice in the Description page of Project Settings.

#include "MapperPointInteractive.h"
#include "MapperContainerBase.h"

#include "Kismet/KismetRenderingLibrary.h"

#include "KulaginStatics.h"

AMapperPointInteractive::AMapperPointInteractive()
{
	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMapperPointInteractive::BeginPlay()
{
	Super::BeginPlay();
}

void AMapperPointInteractive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AMapperPointInteractive::UpdatePlane_Implementation() { return false; }

bool AMapperPointInteractive::UpdateDataFromActor_Implementation() { return false; }

bool AMapperPointInteractive::SetPointIndex_Implementation(int32 IndexIn) { return false; }

bool AMapperPointInteractive::UpdateProjection_Implementation() { return false; }

bool AMapperPointInteractive::SetProjectionEnabled_Implementation(bool bEnabledIn) { return false; }

bool AMapperPointInteractive::UpdateFirstLast_Implementation() { return false; }

bool AMapperPointInteractive::UpdateProjectionComponents_Implementation(USceneComponent* Root, USceneComponent* Decal) { return false; }

bool AMapperPointInteractive::SetFirst_Implementation() { return false; }

bool AMapperPointInteractive::IsFirst_Implementation()
{
	return PointIndex == 0;
}

bool AMapperPointInteractive::IsLast_Implementation()
{
	return ParentContainer && (ParentContainer->PointActors.Num() - 1) == PointIndex;
}

bool AMapperPointInteractive::SetLast_Implementation() { return false; }

bool AMapperPointInteractive::SetNormal_Implementation() { return false; }

bool AMapperPointInteractive::ShowArrow_Implementation()
{
	return false;
}

bool AMapperPointInteractive::HideArrow_Implementation()
{
	return false;
}

float AMapperPointInteractive::GetWaitTime_Implementation() { return 0.f; }

float AMapperPointInteractive::GetEnterTime_Implementation()
{ 
	return EnterTime; 
}

float AMapperPointInteractive::GetLeaveTime_Implementation() {
	return bIsFirstLast ? GetEnterTime() : GetEnterTime() + GetWaitTime();
}

void AMapperPointInteractive::GetPreviousOfClass(AMapperPointBase*& Previous, TSubclassOf<AMapperPointBase> PointClass)
{ 
	Previous = nullptr;

	AMapperPointBase* Current = GetPrevious();
	if (Current == nullptr) return;

	UClass* CurrentClass = Current->GetClass();

	if (CurrentClass == PointClass || CurrentClass->IsChildOf(PointClass))
		Previous = Current;
}

void AMapperPointInteractive::GetNextOfClass(AMapperPointBase*& Next, TSubclassOf<AMapperPointBase> PointClass)
{
	Next = nullptr;

	AMapperPointBase* Current = GetNext();
	if (Current == nullptr) return;

	UClass* CurrentClass = Current->GetClass();

	if (CurrentClass == PointClass || CurrentClass->IsChildOf(PointClass))
		Next = Current;
}

AMapperPointBase* AMapperPointInteractive::GetPrevious_Implementation()
{ 
	return (ParentContainer && ParentContainer->PointActors.IsValidIndex(PointIndex - 1)) ? ParentContainer->PointActors[PointIndex - 1] : nullptr;
}

AMapperPointBase* AMapperPointInteractive::GetNext_Implementation() {
	return (ParentContainer && ParentContainer->PointActors.IsValidIndex(PointIndex - 1)) ? ParentContainer->PointActors[PointIndex - 1] : TargetPoint;
}

bool AMapperPointInteractive::UpdateSpeed_Implementation(float NewTargetTimeIn) { return false; }

bool AMapperPointInteractive::UpdateCrossRadius_Implementation() { return false; }

bool AMapperPointInteractive::UpdateCrossCylinder_Implementation() { return false; }

bool AMapperPointInteractive::UpdateCrossVisibility_Implementation() { return false; }

bool AMapperPointInteractive::UpdatePathVisibility_Implementation() { return false; }