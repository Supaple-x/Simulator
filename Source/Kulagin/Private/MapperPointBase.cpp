// Fill out your copyright notice in the Description page of Project Settings.

#include "MapperPointBase.h"

#include "Kismet/KismetRenderingLibrary.h"

#include "KulaginStatics.h"

AMapperPointBase::AMapperPointBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMapperPointBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMapperPointBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AMapperPointBase::SetMaterial_Implementation(UMaterialInterface* NewMaterial)
{
	return false;
}

void AMapperPointBase::SetDangerZoneMask(UTextureRenderTarget2D* RenderTarget, float StartTime, float EndTime, const TArray<FDangerZone> &Zones)
{
	if (RenderTarget == nullptr)
		return;

	UKismetRenderingLibrary::ClearRenderTarget2D(this, RenderTarget, FLinearColor::Black);

	if (Zones.IsValidIndex(0) == false)
		return;

	for (const FDangerZone& CurrentZone : Zones)
	{
		if (CurrentZone.SecondsOver < StartTime)
			continue;

		if (CurrentZone.SecondsBegin >= EndTime)
			break;

		const float Length = EndTime - StartTime;
		const float SecondsBeginLocal = CurrentZone.SecondsBegin - StartTime;
		const float SecondsOverLocal = CurrentZone.SecondsOver - StartTime;
		
		const float SecondsMin = FMath::Min(SecondsBeginLocal, SecondsOverLocal);
		const float SecondsMax = FMath::Max(SecondsBeginLocal, SecondsOverLocal);

		const float PercentBegin = FMath::Clamp(SecondsMin / Length, 0.f, 1.f);
		const float PercentOver = FMath::Clamp(SecondsMax / Length, 0.f, 1.f);

		DrawDangerZoneMask(RenderTarget, PercentBegin, PercentOver);
	}
}

void AMapperPointBase::DrawDangerZoneMask(UTextureRenderTarget2D* RenderTarget, float StartPercent, float EndPercent)
{
	if (RenderTarget == nullptr)
		return;

	UCanvas* Canvas;
	FVector2D Size;
	FDrawToRenderTargetContext Context;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RenderTarget, Canvas, Size, Context);

	if (Canvas == nullptr)
		return;

	const float ScreenPosX = (StartPercent + EndPercent) * 0.5f * Size.X;
	const float ScreenPosY = 0.f;
	const float ScreenSizeX = 0.01f;
	const float ScreenSizeY = Size.Y;
	const float Thickness = (EndPercent - StartPercent) * 1.f * Size.X;

	FCanvasBoxItem BoxItem(FVector2D(ScreenPosX, ScreenPosY), FVector2D(ScreenSizeX, ScreenSizeY));
	BoxItem.LineThickness = Thickness;
	BoxItem.SetColor(FLinearColor::White);

	Canvas->DrawItem(BoxItem);
}

bool AMapperPointBase::SetNextPoint_Implementation(AMapperPointBase* NextPointIn)
{
	return false;
}

bool AMapperPointBase::UpdateActorFromData_Implementation()
{
	return false;
}

bool AMapperPointBase::SetDataFromList_Implementation(const FMissionPointList& List, const int32 Index)
{
	return false;
}

bool AMapperPointBase::SetDataFromPoint_Implementation(AMapperPointBase* Point)
{
	return false;
}