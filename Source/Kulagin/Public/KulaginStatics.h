// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "KulaginTypes.h"

#include "KulaginStatics.generated.h"

class AMapperGameMode;
class UMapperGameInstance;
class UMapperTileComponent;

/**
 * 
 */
UCLASS()
class KULAGIN_API UKulaginStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/*
	 * 1 -> 01, 2 -> 02, 5 -> 05, and so on
	 */
	static FString FixSingleInt(int32 InInt) { return InInt < 10 ? "0" + FString::FromInt(InInt) : FString::FromInt(InInt); };

	/*
	 * Return string YY-MM-DD_HH-MM-SS
	 */
	UFUNCTION(BlueprintPure)
	static FString GetCurrentTime(int32& Year, int32& Month, int32& Day, int32& Hours, int32& Minutes, int32& Seconds);

	UFUNCTION(BlueprintPure)
	static int32 GetTexture2DDynamicSizeX(UTexture2DDynamic* Texture) { return Texture->SizeX; };

	UFUNCTION(BlueprintPure)
	static int32 GetTexture2DDynamicSizeY(UTexture2DDynamic* Texture) { return Texture->SizeY; };

	UFUNCTION(BlueprintCallable)
	static FMapTileInfo GetMapTile(FLatLon LatLon, int32 Zoom);

	UFUNCTION(BlueprintPure)
	static FLatLon LatLonTopLeft(FLatLon A, FLatLon B);

	UFUNCTION(BlueprintPure)
	static FLatLon LatLonBottomRight(FLatLon A, FLatLon B);

	UFUNCTION(BlueprintPure)
	static FLatLon LatLonCenter(FLatLon A, FLatLon B);

	UFUNCTION(BlueprintCallable)
	static UMapperTileComponent* SpawnMapTile(USceneComponent* Parent, const FIntPoint TileIndex, const FVector RelativeLocation);

	UFUNCTION(BlueprintCallable)
	static void SpawnDefaultTiles(USceneComponent* Parent, FLatLon TopLeft, FLatLon BottomRight);

	UFUNCTION(BlueprintCallable)
	static void GetMinZoomIndex(const FMapTileInfo &Info, const FVector TileCenter, int32 &IndexX, int32 &IndexY);

	UFUNCTION(BlueprintPure)
	static void GetGoogleTile(const FMapTileInfo &Tile, int32 &GoogleX, int32 &GoogleY) { Tile.GetGoogleTile(GoogleX, GoogleY); };

	UFUNCTION(BlueprintPure)
	static void GetMapTileSize(const FMapTileInfo &Tile, float &SizeX, float &SizeY) { Tile.GetWorldSize(SizeX, SizeY); };

	/*
	 * Image corners
	 * 0 - Top Left, 1 - Top Right, 2 - Bottom Left, 3 - Bottom Right
	 */
	UFUNCTION(BlueprintCallable)
	static bool SetOrientActorsFromFile(UPARAM(ref) TArray<FMissionPoint> &Points, TArray<AActor*> NaviPoints, UTexture2DDynamic* Texture, TArray<USceneComponent*> ImageCorners);

	static bool GetOrientValuesFromFile(FBinaryFilePath Path, TArray<double> &Lat, TArray<double> &Lon, TArray<int32> &ImageX, TArray<int32> &ImageY);

	static bool GetOrientValueFromFileTab(FBinaryFilePath Path, TArray<double> &Lat, TArray<double> &Lon, TArray<int32> &ImageX, TArray<int32> &ImageY);

	static bool GetOrientValueFromFileAtAtAt(FBinaryFilePath Path, TArray<double> &Lat, TArray<double> &Lon, TArray<int32> &ImageX, TArray<int32> &ImageY);

	static FString LastPath;

	static FReferenceLatLon Reference;

	UFUNCTION(BlueprintCallable)
	static void SetReference(const FLatLon LatLon) { Reference.SetReference(LatLon); };

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static AMapperGameMode* GetMapperGameMode(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static UMapperGameInstance* GetMapperGameInstance(const UObject* WorldContextObject);

	/*
	 * 1 real life meter = UU / Scale
	 * 1 UU = 1 real life meter * Scale
	 */
	static double Scale;

	static int32 MaxZoom;

	UFUNCTION(BlueprintPure)
	static int32 GetMaxZoom() { return MaxZoom; };

	UFUNCTION(BlueprintCallable)
	static void SetMaxZoom(int32 MaxZoomIn) { MaxZoom = MaxZoomIn; };

	static int32 MinZoom;

	UFUNCTION(BlueprintPure)
	static int32 GetMinZoom() { return MinZoom; };

	UFUNCTION(BlueprintCallable)
	static void SetMinZoom(int32 MinZoomIn) { MinZoom = MinZoomIn; };

	UFUNCTION(BlueprintPure)
	static FLatLon GetLatLonTopLeft() { return FLatLon::TopLeft; };

	UFUNCTION(BlueprintPure)
	static FLatLon GetLatLonBottomRight() { return FLatLon::BottomRight; };

	UFUNCTION(BlueprintPure)
	static FLatLon GetLatLonMin() { return FLatLon::Min; };

	UFUNCTION(BlueprintPure)
	static FLatLon GetLatLonMax() { return FLatLon::Max; };

	static float PixelsPerSecond;

	UFUNCTION(BlueprintPure)
	static float GetPixelsPerSecond() { return PixelsPerSecond; };

	UFUNCTION(BlueprintCallable)
	static void SetPixelsPerSecond(const float NewPixelsPerSecond) { PixelsPerSecond = NewPixelsPerSecond; };

	static float MaxSnappingSeconds;

	UFUNCTION(BlueprintPure)
	static float GetMaxSnappingSeconds() { return MaxSnappingSeconds; };

	UFUNCTION(BlueprintCallable)
	static void SetMaxSnappingSeconds(const float NewMaxSnappingSeconds) { MaxSnappingSeconds = NewMaxSnappingSeconds; };

	UFUNCTION(BlueprintPure)
	static FString GetLastPath() { return LastPath; };

	UFUNCTION(BlueprintCallable)
	static bool AskUserForFile(FBinaryFilePath &Path, FString DialogTitle, FString FileTypes);

	UFUNCTION(BlueprintCallable)
	static TArray<FBinaryFilePath> AskUserForFiles(FString DialogTitle, FString FileTypes);

	UFUNCTION(BlueprintCallable)
	static FString AskUserForDirectory();

	UFUNCTION(BlueprintCallable)
	static FLogPointList GetLogPointsFromFile(FBinaryFilePath Path);

	UFUNCTION(BlueprintCallable)
	static FMissionPointList GetPointsFromFile(FBinaryFilePath Path);

	static TArray<FMissionPointNative> GetPointsNativeFromFile(const TCHAR* Path, FLatLon &TopLeft, FLatLon &BottomRight);

	UFUNCTION(BlueprintCallable)
	static FPlaneMissionPointList GetPlanePointsFromFile(FBinaryFilePath Path);

	static TArray<FPlaneMissionPointNative> GetPlanePointsNativeFromFile(const TCHAR* Path, FLatLon &TopLeft, FLatLon &BottomRight);

	static TArray<FMissionPoint> NativePointsToPoints(TArray<FMissionPointNative> PointsNative);

	static TArray<FPlaneMissionPoint> PlaneNativePointsToPoints(TArray<FPlaneMissionPointNative> PointsNative);

	UFUNCTION(BlueprintCallable)
	static bool SavePointsToFile(FBinaryFilePath Path, TArray<FMissionPoint> Points);

	static bool SavePointsNativeToFile(const TCHAR* Path, TArray<FMissionPointNative> PointsNative);

	UFUNCTION(BlueprintCallable)
	static bool SavePlanePointsToFile(FBinaryFilePath Path, TArray<FPlaneMissionPoint> Points);

	static bool SavePlanePointsNativeToFile(const TCHAR* Path, TArray<FPlaneMissionPointNative> PointsNative);

	static TArray<FMissionPointNative> PointsToNativePoints(TArray<FMissionPoint> Points);

	static TArray<FPlaneMissionPointNative> PlanePointsToNativePoints(TArray<FPlaneMissionPoint> Points);

	/** Offset in Unreal Unints */
	UFUNCTION(BlueprintCallable)
	static void AddMissionPointsOffset(UPARAM(ref) TArray<FMissionPoint> &Points, FVector Offset);

	/** Offset in Unreal Unints */
	UFUNCTION(BlueprintCallable)
	static void AddMissionPointOffset(UPARAM(ref) FMissionPoint &Point, FVector Offset);

	/** Offset in Unreal Unints */
	UFUNCTION(BlueprintCallable)
	static void AddPlaneMissionPointsOffset(UPARAM(ref) TArray<FPlaneMissionPoint> &Points, FVector Offset);

	/** Offset in Unreal Unints */
	UFUNCTION(BlueprintCallable)
	static void AddPlaneMissionPointOffset(UPARAM(ref) FPlaneMissionPoint &Point, FVector Offset);

	static double GetValidDoubleFromString(FString Str);

	static FString GetValidStringFromDouble(double Val);

	UFUNCTION(BlueprintPure)
	static void LatLonToFloat(FLatLon LatLon, float &Lat, float &Lon) { Lat = LatLon.Lat; Lon = LatLon.Lon; };

	UFUNCTION(BlueprintCallable)
	static void SetTargetLat(UPARAM(ref) FMissionPoint &Point, FString Lat);

	UFUNCTION(BlueprintCallable)
	static void SetTargetLon(UPARAM(ref) FMissionPoint &Point, FString Lon);

	UFUNCTION(BlueprintCallable)
	static void SetPoiLat(UPARAM(ref) FMissionPoint &Point, FString Lat);

	UFUNCTION(BlueprintCallable)
	static void SetPoiLon(UPARAM(ref) FMissionPoint &Point, FString Lon);

	UFUNCTION(BlueprintPure)
	static FLatLon LatLonFromFloat(const float Lat, const float Lon) { return FLatLon(Lat, Lon); };

	UFUNCTION(BlueprintPure)
	static FString GetTargetLat(const FMissionPoint &Point);

	UFUNCTION(BlueprintPure)
	static FString GetTargetLon(const FMissionPoint &Point);

	UFUNCTION(BlueprintPure)
	static FString GetPoiLat(const FMissionPoint &Point);

	UFUNCTION(BlueprintPure)
	static FString GetPoiLon(const FMissionPoint &Point);

	UFUNCTION(BlueprintPure)
	static FString GetBinaryFullPath(const FBinaryFilePath &Path);

	UFUNCTION(BlueprintPure)
	static FReferenceLatLon GetReferenceLatLon(const FMissionPoint &Point);

	UFUNCTION(BlueprintPure)
	static bool IsReferenceLatLonValid(const FReferenceLatLon &ReferenceIn);

	UFUNCTION(BlueprintCallable)
	static FVector LatLonToUE(const FLatLon LatLon, float StartAlt);

	UFUNCTION(BlueprintCallable)
	static FVector WGS84ToUE(const FMissionPoint &Point, float StartAlt);

	UFUNCTION(BlueprintCallable)
	static FVector LogPointToUE(const FLogPoint &Point, float StartAlt);

	UFUNCTION(BlueprintCallable)
	static FVector PlaneMissionPointToUE(const FPlaneMissionPoint &Point, float StartAlt);

	static FVector WGS84ToUE(double Lat, double LatReference, double Lon, double LonReference, float Alt, float StartAlt);

	UFUNCTION(BlueprintCallable)
	static FLatLon UEToLatLon(FVector Loc);

	UFUNCTION(BlueprintCallable)
	static void UEToWGS84(UPARAM(ref) FMissionPoint &Point, FVector Loc, float StartAlt);

	UFUNCTION(BlueprintCallable)
	static void UEToLogPoint(UPARAM(ref) FLogPoint &Point, FVector Loc, float StartAlt);

	UFUNCTION(BlueprintCallable)
	static void UEToPlaneMissionPoint(UPARAM(ref) FPlaneMissionPoint &Point, FVector Loc, float StartAlt);

	static void UEToWGS84(FVector Loc, double &Lat, double LatReference, double &Lon, double LonReference, float &Alt, float StartAlt);

	/** Converts real life meters to Unreal Units */
	UFUNCTION(BlueprintPure)
	static float LifeToUU(const float LifeValue) { return LifeValue * Scale; };

	/** Converts Unreal Units to real life meters */
	UFUNCTION(BlueprintPure)
	static float UUToLife(const float UUValue) { return UUValue / Scale; };

	UFUNCTION(BlueprintPure)
	static bool IsValidDroneData(const FDroneData Data) { return Data.IsValid(); };

	UFUNCTION(BlueprintPure)
	static int32 GetZoomMaxIndex();

};
