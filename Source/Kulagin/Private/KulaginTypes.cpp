// Fill out your copyright notice in the Description page of Project Settings.

#include "KulaginTypes.h"

#include "Runtime/Engine/Classes/Kismet/KismetStringLibrary.h"

#include "KulaginStatics.h"
#include "GlobalMercator.hpp"

#include "Kulagin.h"

const FLatLon FLatLon::TopLeft(95., -185.);
const FLatLon FLatLon::BottomRight(-95., 185.);
const FLatLon FLatLon::Min(-95., -185.);
const FLatLon FLatLon::Max(95., 185.);

FMissionPointNative::FMissionPointNative()
{
	targetLat = 0.;
	targetLon = 0.;
	targetAlt = 0.f;
	targetRadius = 1.f;
	loiterTime = 0;
	maxHorizSpeed = 10.f;
	maxVertSpeed = 2.f;
	poiLat = 0.;
	poiLon = 0.;
	poiHeading = 0.f;
	poiAltitude = 0.f;
	flags = 0;
	photo = 0;
	panoSectorsCount = 0;
	panoDeltaAngle = 0.f;
	poiPitch = 0.f;
	poiRoll = 0.f;
	type = 3;
}

FMissionPointNative::FMissionPointNative(FMissionPoint Point)
{
	targetLat = Point.TargetLatLon.Lat;
	targetLon = Point.TargetLatLon.Lon;
	targetAlt = Point.targetAlt;
	targetRadius = Point.targetRadius;
	loiterTime = Point.loiterTime;
	maxHorizSpeed = Point.maxHorizSpeed;
	maxVertSpeed = Point.maxVertSpeed;
	poiLat = Point.poiLat;
	poiLon = Point.poiLon;
	poiHeading = Point.poiHeading;
	poiAltitude = Point.poiAltitude;
	flags = Point.flags.ToMask();
	photo = Point.photo;
	panoSectorsCount = Point.panoSectorsCount;
	panoDeltaAngle = Point.panoDeltaAngle;
	poiPitch = Point.poiPitch;
	poiRoll = Point.poiRoll;
	type = uint8(Point.type);
}

int32 FMissionPointNative::GetSize() const
{
	int32 TotalSize = sizeof(targetLat);
	TotalSize += sizeof(targetLon);
	TotalSize += sizeof(targetAlt);
	TotalSize += sizeof(targetRadius);
	TotalSize += sizeof(loiterTime);
	TotalSize += sizeof(maxHorizSpeed);
	TotalSize += sizeof(maxVertSpeed);
	TotalSize += sizeof(poiLat);
	TotalSize += sizeof(poiLon);
	TotalSize += sizeof(poiHeading);
	TotalSize += sizeof(poiAltitude);
	TotalSize += sizeof(flags);
	TotalSize += sizeof(photo);
	TotalSize += sizeof(panoSectorsCount);
	TotalSize += sizeof(panoDeltaAngle);
	TotalSize += sizeof(poiPitch);
	TotalSize += sizeof(poiRoll);
	TotalSize += sizeof(type);
	return TotalSize;
}

FMissionPoint::FMissionPoint()
{
	TargetLatLon = FLatLon(0., 0.);
	targetAlt = 0.f;
	targetRadius = 1.f;
	loiterTime = 0;
	maxHorizSpeed = 10.f;
	maxVertSpeed = 2.f;
	poiLat = 0.;
	poiLon = 0.;
	poiHeading = 0.f;
	poiAltitude = 0.f;
	flags = FNaviFlags(0b0000'0010'0000);
	photo = 0;
	panoSectorsCount = 0;
	panoDeltaAngle = 0.f;
	poiPitch = 0.f;
	poiRoll = 0.f;
	type = ENaviPointType::NAVI_POINT_SIMPLE;
}

FMissionPoint::FMissionPoint(FMissionPointNative NativePoint)
{
	TargetLatLon.Lat = NativePoint.targetLat;
	TargetLatLon.Lon = NativePoint.targetLon;
	targetAlt = NativePoint.targetAlt;
	targetRadius = NativePoint.targetRadius;
	loiterTime = NativePoint.loiterTime;
	maxHorizSpeed = NativePoint.maxHorizSpeed;
	maxVertSpeed = NativePoint.maxVertSpeed;
	poiLat = NativePoint.poiLat;
	poiLon = NativePoint.poiLon;
	poiHeading = NativePoint.poiHeading;
	poiAltitude = NativePoint.poiAltitude;
	flags = FNaviFlags(NativePoint.flags);
	photo = NativePoint.photo;
	panoSectorsCount = NativePoint.panoSectorsCount;
	panoDeltaAngle = NativePoint.panoDeltaAngle;
	poiPitch = NativePoint.poiPitch;
	poiRoll = NativePoint.poiRoll;
	type = ENaviPointType(NativePoint.type);
}

FBinaryFilePath::FBinaryFilePath()
{
	// "0123456789"
	// "ab/name.ts"

	PathOnly = L"";
	Filename = L"Empty";
	Extension = L"null";
}

FBinaryFilePath::FBinaryFilePath(FString InFullPath)
{
	InFullPath = FPaths::ConvertRelativePathToFull(InFullPath);
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: FBinaryFilePath: InFullPath = %s"), *InFullPath);
	int32 LastSlashIndex = 0;
	int32 LastDotIndex = 0;
	if (InFullPath.FindLastChar(L'/', LastSlashIndex) && InFullPath.FindLastChar(L'.', LastDotIndex))
	{
		PathOnly = UKismetStringLibrary::GetSubstring(InFullPath, 0, LastSlashIndex);

		Filename = UKismetStringLibrary::GetSubstring(InFullPath, LastSlashIndex + 1, LastDotIndex - LastSlashIndex - 1);

		Extension = UKismetStringLibrary::GetSubstring(InFullPath, LastDotIndex + 1, InFullPath.Len() - LastDotIndex - 1);

		UE_LOG(LogTemp, Warning, TEXT("Kulagin: FBinaryFilePath: PathOnly = %s, Filename = %s, Extension = %s"), *PathOnly, *Filename, *Extension);
	}
}

FString FBinaryFilePath::GetPathOnly() const
{
	return PathOnly;
}

FString FBinaryFilePath::GetFilename() const
{
	return Filename;
}

FString FBinaryFilePath::GetFullPath() const
{
	return PathOnly + L'/' + Filename + L'.' + Extension;
}

FLogPoint::FLogPoint(float TimeIn, double LatIn, double LonIn,
	float AltIn, float RollIn, float PitchIn, float YawIn)
{
	Time = TimeIn;
	Lat = LatIn;
	Lon = LonIn;
	Alt = AltIn;
	Roll = RollIn;
	Pitch = PitchIn;
	Yaw = YawIn;
}

FMapTileInfo::FMapTileInfo(const double Lat, const double Lon, const int32 ZoomIn)
{
	Zoom = ZoomIn;
	//int LocalTileX;
	//int LocalTileY;
	//int GoogleX = 1244;
	//int GoogleY = 736;
	double MinLat = 44.9648;
	double MinLon = 38.6719;
	double MaxLat = 45.089;
	double MaxLon = 38.8477;

	GlobalMercator::WGS84ToTile(Lat, Lon, TMSTileX, TMSTileY, Zoom);

	//GlobalMercator::TileTMSToGoogle(LocalTileX, LocalTileY, GoogleX, GoogleY, Zoom);

	GlobalMercator::WGS84Bounds(TMSTileX, TMSTileY, MinLat, MinLon, MaxLat, MaxLon, Zoom);

	TopLeft.Lat = MaxLat;
	TopLeft.Lon = MinLon;

	TopRight.Lat = MaxLat;
	TopRight.Lon = MaxLon;

	BottomLeft.Lat = MinLat;
	BottomLeft.Lon = MinLon;

	BottomRight.Lat = MinLat;
	BottomRight.Lon = MaxLon;

	Center.Lat = MinLat + (MaxLat - MinLat) * 0.5;
	Center.Lon = MinLon + (MaxLon - MinLon) * 0.5;
}

void FMapTileInfo::GetGoogleTile(int32 &GoogleTileX, int32 &GoogleTileY) const
{
	GlobalMercator::TileTMSToGoogle(TMSTileX, TMSTileY, GoogleTileX, GoogleTileY, Zoom);
}

void FMapTileInfo::GetWorldSize(float &SizeX, float &SizeY) const
{
	const FVector LocTopLeft = UKulaginStatics::LatLonToUE(TopLeft, 0.f);
	const FVector LocBottomRight = UKulaginStatics::LatLonToUE(BottomRight, 0.f);

	SizeX = FMath::Abs(LocTopLeft.X - LocBottomRight.X);
	SizeY = FMath::Abs(LocTopLeft.Y - LocBottomRight.Y);
}