// Fill out your copyright notice in the Description page of Project Settings.

#include "MapperContainerBase.h"

#include "MapperGameMode.h"
#include "MapperDroneBase.h"
#include "MapperPointBase.h"
#include "MapperPointInteractive.h"

#include "KulaginStatics.h"

AMapperContainerBase::AMapperContainerBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AMapperContainerBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMapperContainerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMapperContainerBase::GetPointsOfClass(TSubclassOf<AMapperPointBase> PointClass, TArray<AMapperPointBase*>& OutPoints)
{
	OutPoints.Empty();

	for (AMapperPointBase* Current : PointActors)
	{
		if (Current == nullptr) continue;
		
		UClass* CurrentClass = Current->GetClass();

		if (CurrentClass == PointClass || CurrentClass->IsChildOf(PointClass))
			OutPoints.Add(Current);
	}
}

float AMapperContainerBase::GetDangerZoneRadiusUU() const
{
	return DangerZoneRadius * 0.01 * UKulaginStatics::Scale;
}

bool AMapperContainerBase::UpdateDangerZones_Implementation()
{
    if (ContainerType != EPointContainerType::PCT_Mission)
		return false;

	DangerZones.Empty();

	const float CurrentRadius = GetDangerZoneRadiusUU();
	const float CurrentTotalTime = GetTotalTime();

	bool bIsOpenDangerZone = false;
	bool bIsCrossPath = false;
	bool bIsCrossScene = false;
	float LastTime = 0.f;

	UWorld* CurrentWorld = GetWorld();

	const TArray<AMapperContainerBase*> AllContainers = UKulaginStatics::GetMapperGameMode(this)->PointContainers;
	AActor* CurrentSceneActor = UKulaginStatics::GetMapperGameMode(this)->GetSceneActor();
	TArray<AActor*> CurrentSceneAttachActors;
	if (CurrentSceneActor)
	{
		CurrentSceneActor->GetAttachedActors(CurrentSceneAttachActors);
		CurrentSceneAttachActors.Add(CurrentSceneActor);
	}
	TInlineComponentArray<UPrimitiveComponent*> AllComponents;
	for (AActor* CurrentActor : CurrentSceneAttachActors)
	{
		if (CurrentActor == nullptr)
			continue;
		TInlineComponentArray<UPrimitiveComponent*> Temp;
		CurrentActor->GetComponents(Temp);
		AllComponents.Append(Temp);
	}

    TArray<AActor*> IgnoredActors;
    for (AMapperContainerBase* CurrentContainer : AllContainers)
    {
        if (CurrentContainer == nullptr)
            continue;

        IgnoredActors.Add(CurrentContainer);
        IgnoredActors.Append(CurrentContainer->GetActorsIgnoreCrossing());
    }

	for (float CurrentTime = 0.f; CurrentTime < CurrentTotalTime; CurrentTime += 0.05f)
	{
		const TArray<FVector> ThisSwarm = GetSwarmLocationsAtTime(CurrentTime);
		bool IsAnyNear = false;
		bool bIsCrossPathTime = false;

        for (const FVector& ThisLocation : ThisSwarm)
        {
            if (bIsCrossPathTime == false && AllContainers.Num() > 1)
            {
                bIsCrossPathTime = FindCrossPath(ThisLocation, AllContainers, CurrentTime);
                IsAnyNear |= bIsCrossPathTime;
                bIsCrossPath |= bIsCrossPathTime;
            }

            /*if (bIsCrossScene == false)
            {
                bIsCrossScene = FindCrossScene(ThisLocation, IgnoredActors);
                IsAnyNear |= bIsCrossScene;
            }*/

            if (bIsCrossPath && bIsCrossScene) break;

            /*if (false)
            {
                FVector ClosestPoint;
                //for (AActor* CurrentCrossActor : CurrentSceneAttachActors)
                //{
                    //if (CurrentCrossActor == nullptr)
                    //	continue;

                    //UE_LOG(LogTemp, Warning, TEXT("Kulagin: UpdateDangerZones: CurrentCrossActor = %s, CurrentRadius = %f"), *CurrentCrossActor->GetName(), CurrentRadius);

                    //const float Dist = CurrentCrossActor->ActorGetDistanceToCollision(CurrentLocation, ECollisionChannel::ECC_Visibility, ClosestPoint);

                    //TSet<UActorComponent*> Comps = CurrentCrossActor->GetComponents();

                for (UPrimitiveComponent* Prim : AllComponents)
                {
                    //if (CurrentComponent == nullptr)
                    //	continue;

                    //UPrimitiveComponent* Prim = Cast< UPrimitiveComponent>(CurrentComponent);

                    if (Prim == nullptr)
                        continue;

                    //UE_LOG(LogTemp, Warning, TEXT("Kulagin: UpdateDangerZones: CurrentComponent = %s"), *CurrentComponent->GetName());

                    const float Dist = Prim->GetDistanceToCollision(ThisLocation, ClosestPoint);

                    //UE_LOG(LogTemp, Warning, TEXT("Kulagin: UpdateDangerZones: Dist = %f"), Dist);

                    if (Dist >= 0.f && Dist <= CurrentRadius)
                    {
                        //UE_LOG(LogTemp, Warning, TEXT("Kulagin: UpdateDangerZones: VALID SCENE CROSS"));

                        IsAnyNear = true;
                        bIsCrossScene = true;
                        break;
                    }
                }
                //if (IsAnyNear)
                //	break;
            //}
            }*/
        }

		if (bIsOpenDangerZone && IsAnyNear == false)
		{
			DangerZones.Add(FDangerZone(LastTime, CurrentTime, bIsCrossPath, bIsCrossScene));
			bIsOpenDangerZone = false;
			bIsCrossPath = false;
			bIsCrossScene = false;
		}
		else if (bIsOpenDangerZone == false && IsAnyNear)
		{
			LastTime = CurrentTime;
			bIsOpenDangerZone = true;
		}
	}
	if (bIsOpenDangerZone)
	{
		DangerZones.Add(FDangerZone(LastTime, CurrentTotalTime, bIsCrossPath, bIsCrossScene));
	}
	return true;
}

bool AMapperContainerBase::FindCrossPath_Implementation(const FVector& Loc, const TArray<AMapperContainerBase*>& Containers, const float TimeIn)
{
    const float CurrentRadius = GetDangerZoneRadiusUU();
    for (AMapperContainerBase* CurrentContainer : Containers)
    {
        if (CurrentContainer == nullptr || CurrentContainer == this || CurrentContainer->ContainerType != EPointContainerType::PCT_Mission)
            continue;

        const TArray<FVector> OtherSwarm = CurrentContainer->GetSwarmLocationsAtTime(TimeIn);
        for (const FVector& OtherLocation : OtherSwarm)
        {
            if (FVector::PointsAreNear(Loc, OtherLocation, CurrentRadius))
            {
                return true;
            }
        }
    }
    return false;
}

bool AMapperContainerBase::FindCrossScene_Implementation(const FVector& Loc, const TArray<AActor*>& IgnoredActors)
{
    FCollisionQueryParams TraceParams = FCollisionQueryParams(L"MapperTrace", true);
    TraceParams.bTraceComplex = true;
    TraceParams.AddIgnoredActors(IgnoredActors);

    UWorld* CurrentWorld = GetWorld();
    const float CurrentRadius = GetDangerZoneRadiusUU();

    FHitResult OutHit;
    const FVector StartLocation = Loc + FVector(0.f, 0.f, 1.f) * CurrentRadius;
    const FVector EndLocation = Loc + FVector(0.f, 0.f, -1.f) * CurrentRadius;
    //const FCollisionShape Shape = FCollisionShape::MakeSphere(CurrentRadius);
    const FCollisionShape Shape = FCollisionShape::MakeCapsule(CurrentRadius, 0.5f);

    //UE_LOG(LogTemp, Warning, TEXT("Kulagin: UpdateDangerZones: SweepSingleByChannel v2, time = %f"), CurrentTime);

    //if (CurrentWorld && CurrentWorld->LineTraceSingleByChannel(OutHit, CurrentLocation, NewLocation, ECC_Visibility, TraceParams))
    if (CurrentWorld && CurrentWorld->SweepSingleByChannel(OutHit, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, Shape, TraceParams))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Kulagin: UpdateDangerZones: SweepSingleByChannel VALID"));

        return true;
    }
    return false;
}

float AMapperContainerBase::GetTotalTime_Implementation() const
{
	return 0.f;
}

bool AMapperContainerBase::SetPointsMaterial_Implementation()
{
	return false;
}
	
bool AMapperContainerBase::SetPointsColor_Implementation(FLinearColor NewColor)
{
	return false;
}

bool AMapperContainerBase::SetMovingActor_Implementation(float NewTime)
{
	if (bIsRised == false) RiseMovingActor();
	return false;
}

FVector AMapperContainerBase::GetLocationAtTime_Implementation(float TimeIn)
{
	return FVector::ZeroVector;
}

TArray<FVector> AMapperContainerBase::GetSwarmLocationsAtTime_Implementation(float TimeIn)
{
    const FVector CurrentLoc = GetLocationAtTime(TimeIn);
    if (bIsSwarm == false)
    {
        return { CurrentLoc };
    }
    TArray<FVector> Swarm = SwarmLocationOffsets;
    for (FVector& el : Swarm)
    {
        el += CurrentLoc;
    }
    return Swarm;
}

bool AMapperContainerBase::StartMoving_Implementation()
{
	if (bIsRised == false) RiseMovingActor();
	return false;
}

bool AMapperContainerBase::SetTimelineIsHighlighted_Implementation(bool bNewIsHighlighted)
{
	return bNewIsHighlighted;
}

bool AMapperContainerBase::SetWorldTrackIsHighlighted_Implementation(bool bNewIsHighlighted)
{
	return bNewIsHighlighted;
}

bool AMapperContainerBase::TogglePathVisibility_Implementation()
{
	return SetPathVisibility(!bIsPathVisible);
}
	
bool AMapperContainerBase::SetPathVisibility_Implementation(bool bNewIsVisible)
{
	bIsPathVisible = bNewIsVisible;
	return bIsPathVisible;
}

bool AMapperContainerBase::ToggleMovingActorVisibility_Implementation()
{
	return SetMovingActorVisibility(!bIsMovingActorVisible);
}

bool AMapperContainerBase::SetMovingActorVisibility_Implementation(bool bNewIsVisible)
{
	bIsMovingActorVisible = bNewIsVisible;
	return bIsMovingActorVisible;
}

bool AMapperContainerBase::ToggleDangerZoneVisibility_Implementation()
{
	return SetDangerZoneVisibility(!bIsDangerZoneVisible);
}

bool AMapperContainerBase::SetDangerZoneVisibility_Implementation(bool bNewIsVisible)
{
	bIsDangerZoneVisible = bNewIsVisible;
	return bIsDangerZoneVisible;
}

bool AMapperContainerBase::TogglePaused_Implementation()
{
	return SetPaused(!bIsPaused);
}

bool AMapperContainerBase::SetPaused_Implementation(bool bNewIsPaused)
{
	bIsPaused = bNewIsPaused;
	if (bIsPaused == false && bIsRised == false) RiseMovingActor();
	return bIsPaused;
}

bool AMapperContainerBase::RemovePath_Implementation()
{
	return false;
}

TArray<AActor*> AMapperContainerBase::GetActorsIgnoreCrossing_Implementation() const
{
	TArray<AActor*> Temp;
	Temp.Append(PointActors);
	Temp.Add(DroneActor);
	return Temp;
}

bool AMapperContainerBase::IsTimeInDangerZone_Implementation(float InTime) const
{
	if (bIsPaused) return false;
	for (const FDangerZone &CurrentZone : DangerZones)
	{
		if (CurrentZone.SecondsBegin <= InTime && InTime <= CurrentZone.SecondsOver)
			return true;
	}
	return false;
}

bool AMapperContainerBase::DropMovingActor_Implementation()
{
	if (bIsRised == false)
		return true;

	bIsRised = false;
	SetPaused(true);
	return DroneActor && DroneActor->DropMovingActor();
}

bool AMapperContainerBase::RiseMovingActor_Implementation()
{
	if (bIsRised)
		return true;

	bIsRised = true;
	return DroneActor && DroneActor->RiseMovingActor();
}

bool AMapperContainerBase::SavePath_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("Kulagin: Container: SavePath NOT IMPLEMENTED"));

	return false;
}

bool AMapperContainerBase::CopyPath_Implementation(FVector Offset)
{
	UE_LOG(LogTemp, Error, TEXT("Kulagin: Container: CopyPath NOT IMPLEMENTED"));

	return false;
}

bool AMapperContainerBase::ShiftPath_Implementation(FVector Offset)
{
	for (AMapperPointBase* Current : PointActors)
	{
		if (AMapperPointInteractive* CurrentInteractive = Cast<AMapperPointInteractive>(Current))
		{
			CurrentInteractive->AddActorWorldOffset(Offset);
			CurrentInteractive->UpdateDataFromActor();
		}
	}
	return true;
}