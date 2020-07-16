// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"

#include "KulaginTypes.generated.h"

#define  NAVI_MISS_FLAG_NONE                       0b0000'0000'0000     //Сброс всех флагов (или их отсутствие)
#define  NAVI_MISS_FLAG_RELATIVE_ALT               0b0000'0000'0001     //0 - указанная высота исчисляется относительно точки взлёта 1 - указанная цифра является дельтой к предыдущей целевой высоте
#define  NAVI_MISS_FLAG_PANO_PITCH                 0b0000'0000'0010     //1 - панорамная съемка будет происходить с приращением угла по оси Pitch Используется с точками типа Панорама
#define  NAVI_MISS_FLAG_PANO_ROLL                  0b0000'0000'0100     //1 - панорамная съемка будет происходить с приращением угла по оси Roll Используется с точками типа Панорама
#define  NAVI_MISS_FLAG_PANO_YAW                   0b0000'0000'1000     //1 - панорамная съемка будет происходить с приращением угла по оси Yaw (изменение курса) Используется с точками типа Панорама

#define  NAVI_MISS_FLAG_FRRE_AND_RETURN_HEADING    0b0000'0001'0000     //NAVI_MISS_FLAG_FRRE_AND_RETURN_HEADING NAVI_MISS_FLAG_FIXED_HEADING
																		//00 - Свободное управление курсом при выполнении мисси
																		//01 - Разрешать управлять курсом, но возвращать к указанному в миссии
																		//10 - Запретить ручное управление курсом в миссии
																		//11 - Не изменять курс (игнорировать команду смены курса при загрузке)
#define  NAVI_MISS_FLAG_FIXED_HEADING              0b0000'0010'0000     //

#define  NAVI_MISS_FLAG_RELATIVE_COORD             0b0000'0100'0000     //1 - означает исчисление координат целевой точки относительно ранее заданной в миссии базовой точки
#define  NAVI_MISS_FLAG_SET_AUTO_HEADING_TO_TARGET 0b0000'1000'0000     //1 - означает, что при загрузке нового задания (целевой точки) аппарат развернётся курсом на цель
#define  NAVI_MISS_FLAG_SYNC_TIME                  0b0001'0000'0000     //1 - означает, что через время, указанное в loiterTime, в любом случае будет произведён переход на следующую точку миссии. При этом не важно, достигнута уже цель или нет. Актуально для сихромиссий (шоу дронов и т.п.)

struct FMissionPointNative
{
	double   targetLat;
	double   targetLon;
	float    targetAlt;
	float    targetRadius;
	uint32   loiterTime;
	float    maxHorizSpeed;
	float    maxVertSpeed;
	double   poiLat;
	double   poiLon;
	float    poiHeading;
	float    poiAltitude;
	uint32   flags;
	uint8    photo;
	uint8    panoSectorsCount;
	float    panoDeltaAngle;
	float    poiPitch;
	float    poiRoll;
	uint8    type;

	int32 GetSize() const;

	FMissionPointNative();

	FMissionPointNative(struct FMissionPoint Point);
};

struct FPlaneMissionPointNative
{
	double   Lat;
	double   Lon;
	float    Alt;
	float	 Time;

	int32 GetSize() const;

	FPlaneMissionPointNative();

	FPlaneMissionPointNative(struct FPlaneMissionPoint Point);
};

UENUM(BlueprintType)
enum class ESceneType : uint8
{
	ST_Empty			UMETA(DisplayName = "Empty"),
	ST_Mesh				UMETA(DisplayName = "Mesh"),
	ST_Image			UMETA(DisplayName = "Image"),
	ST_Web				UMETA(DisplayName = "Web")
};

UENUM(BlueprintType)
enum class EImportStage : uint8
{
	ST_Ready					UMETA(DisplayName = "Ready"),
	ST_ImportingScene			UMETA(DisplayName = "ImportingScene"),
	ST_ImportingFromWeb			UMETA(DisplayName = "ImportingFromWeb"),
	ST_Orientating				UMETA(DisplayName = "Orientating")
};

UENUM(BlueprintType)
enum class ENaviPointType : uint8
{
	NAVI_POINT_HOME = 1			UMETA(DisplayName = "Point Home"),
	NAVI_POINT_TAKEOFF = 2		UMETA(DisplayName = "Point Takeoff"),
	NAVI_POINT_SIMPLE = 3		UMETA(DisplayName = "Point Simple"),
	NAVI_POINT_CIRCLE = 4		UMETA(DisplayName = "Point Circle"),
	NAVI_POINT_TO_LAND = 5		UMETA(DisplayName = "Point ToLand"),
	NAVI_POINT_PANORAMA = 6		UMETA(DisplayName = "Point Panorama"),
	NAVI_POINT_SET_BASE = 7		UMETA(DisplayName = "Point SetBase"),
	NAVI_POINT_NON_STOP = 8		UMETA(DisplayName = "Point NonStop")
};

UENUM(BlueprintType)
enum class ETileImageState : uint8
{
	TIS_None					UMETA(DisplayName = "None"),
	TIS_MinZoom					UMETA(DisplayName = "Min Zoom"),
	TIS_MaxZoom					UMETA(DisplayName = "Max Zoom")
};

UENUM(BlueprintType)
enum class ENaviFlagHeading : uint8
{
	Free = 0,
	BackToTrack = 1,
	Decline = 2,
	NotChangeTrack = 3
};

UENUM(BlueprintType)
enum class EPointContainerType : uint8
{
	PCT_None				UMETA(DisplayName = "None"),
	PCT_Mission				UMETA(DisplayName = "Mission"),
	PCT_Log					UMETA(DisplayName = "Log"),
	PCT_Orient				UMETA(DisplayName = "Orient"),
	PCT_PlaneMission		UMETA(DisplayName = "Plane Mission")
};

UENUM(BlueprintType)
enum class EDroneActorType : uint8
{
	DAT_Skeletal				UMETA(DisplayName = "Skeletal"),
	DAT_Static					UMETA(DisplayName = "Static"),
	DAT_Class					UMETA(DisplayName = "Class")
};

UENUM(BlueprintType)
enum class ELevelType : uint8
{
	LT_Sandbox				UMETA(DisplayName = "Sandbox"),
	LT_IPU					UMETA(DisplayName = "IPU"),
	LT_ERA					UMETA(DisplayName = "ERA")
};

USTRUCT(BlueprintType)
struct FNaviFlags
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)	bool RelativeAlt = false;
	UPROPERTY(BlueprintReadWrite)	bool PanoPitch = false;
	UPROPERTY(BlueprintReadWrite)	bool PanoRoll = false;
	UPROPERTY(BlueprintReadWrite)	bool PanoYaw = false;
	UPROPERTY(BlueprintReadWrite)	ENaviFlagHeading Heading = ENaviFlagHeading::Free;
	UPROPERTY(BlueprintReadWrite)	bool RelativeCoord = false;
	UPROPERTY(BlueprintReadWrite)	bool AutoHeading = false;
	UPROPERTY(BlueprintReadWrite)	bool SyncTime = false;

	FNaviFlags(const uint32 Mask = 0)
	{
		RelativeAlt = NAVI_MISS_FLAG_RELATIVE_ALT & Mask;
		PanoPitch = NAVI_MISS_FLAG_PANO_PITCH & Mask;
		PanoRoll = NAVI_MISS_FLAG_PANO_ROLL & Mask;
		PanoYaw = NAVI_MISS_FLAG_PANO_YAW & Mask;
		switch (Mask & (NAVI_MISS_FLAG_FRRE_AND_RETURN_HEADING | NAVI_MISS_FLAG_FIXED_HEADING))
		{
		case 0: Heading = ENaviFlagHeading::Free; break;
		case NAVI_MISS_FLAG_FRRE_AND_RETURN_HEADING: Heading = ENaviFlagHeading::BackToTrack; break;
		case NAVI_MISS_FLAG_FIXED_HEADING: Heading = ENaviFlagHeading::Decline; break;
		default: Heading = ENaviFlagHeading::NotChangeTrack;
		}
		RelativeCoord = NAVI_MISS_FLAG_RELATIVE_COORD & Mask;
		AutoHeading = NAVI_MISS_FLAG_SET_AUTO_HEADING_TO_TARGET & Mask;
		SyncTime = NAVI_MISS_FLAG_SYNC_TIME & Mask;
	}

	uint32 ToMask() const
	{
		uint32 Mask = 0;
		Mask |= RelativeAlt ? NAVI_MISS_FLAG_RELATIVE_ALT : 0;
		Mask |= PanoPitch ? NAVI_MISS_FLAG_PANO_PITCH : 0;
		Mask |= PanoRoll ? NAVI_MISS_FLAG_PANO_ROLL : 0;
		Mask |= PanoYaw ? NAVI_MISS_FLAG_PANO_YAW : 0;
		switch (Heading)
		{
		case ENaviFlagHeading::Free: Mask |= 0; break;
		case ENaviFlagHeading::BackToTrack: Mask |= NAVI_MISS_FLAG_FRRE_AND_RETURN_HEADING; break;
		case ENaviFlagHeading::Decline: Mask |= NAVI_MISS_FLAG_FIXED_HEADING; break;
		default: Mask |= NAVI_MISS_FLAG_FRRE_AND_RETURN_HEADING | NAVI_MISS_FLAG_FIXED_HEADING;
		}
		Mask |= RelativeCoord ? NAVI_MISS_FLAG_RELATIVE_COORD : 0;
		Mask |= AutoHeading ? NAVI_MISS_FLAG_SET_AUTO_HEADING_TO_TARGET : 0;
		Mask |= SyncTime ? NAVI_MISS_FLAG_SYNC_TIME : 0;
		return Mask;
	}
};

USTRUCT(BlueprintType)
struct FLatLon
{
	GENERATED_BODY()

	double Lat;
	double Lon;

	FLatLon(const double LatIn = 0., const double LonIn = 0.) : Lat(LatIn), Lon(LonIn) {};

	static const FLatLon TopLeft;
	static const FLatLon BottomRight;
	static const FLatLon Min;
	static const FLatLon Max;
};

USTRUCT(BlueprintType)
struct FMissionPoint
{
	GENERATED_BODY()

									double   targetLat;
									double   targetLon;
	UPROPERTY(BlueprintReadWrite)	FLatLon  TargetLatLon;
	UPROPERTY(BlueprintReadWrite)	float    targetAlt;
	UPROPERTY(BlueprintReadWrite)	float    targetRadius;
	UPROPERTY(BlueprintReadWrite)	int32    loiterTime;
	UPROPERTY(BlueprintReadWrite)	float    maxHorizSpeed;
	UPROPERTY(BlueprintReadWrite)	float    maxVertSpeed;
									double   poiLat;
									double   poiLon;
	UPROPERTY(BlueprintReadWrite)	float    poiHeading;
	UPROPERTY(BlueprintReadWrite)	float    poiAltitude;
	UPROPERTY(BlueprintReadWrite)	FNaviFlags   flags;
	UPROPERTY(BlueprintReadWrite)	uint8    photo;
	UPROPERTY(BlueprintReadWrite)	uint8    panoSectorsCount;
	UPROPERTY(BlueprintReadWrite)	float    panoDeltaAngle;
	UPROPERTY(BlueprintReadWrite)	float    poiPitch;
	UPROPERTY(BlueprintReadWrite)	float    poiRoll;
	UPROPERTY(BlueprintReadWrite)	ENaviPointType    type;

	FMissionPoint();

	FMissionPoint(FMissionPointNative NativePoint);
};

USTRUCT(BlueprintType)
struct FPlaneMissionPoint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)	FLatLon  LatLon;
	UPROPERTY(BlueprintReadWrite)	float    Alt;
	UPROPERTY(BlueprintReadWrite)	float    Time;

	FPlaneMissionPoint();

	FPlaneMissionPoint(FPlaneMissionPointNative NativePoint);
};

USTRUCT(BlueprintType)
struct FBinaryFilePath
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)	FString   PathOnly;
	UPROPERTY(BlueprintReadWrite)	FString   Filename;
	UPROPERTY(BlueprintReadWrite)	FString   Extension;

	FBinaryFilePath();

	FBinaryFilePath(FString InFullPath);

	FString GetPathOnly() const;
	FString GetFilename() const;
	FString GetFullPath() const;
};

USTRUCT(BlueprintType)
struct FReferenceLatLon
{
	GENERATED_BODY()

	double   LatReference;
	double   LonReference;
	bool	 bIsValid;

	FReferenceLatLon() : LatReference(0.), LonReference(0.), bIsValid(false) {};

	FReferenceLatLon(const double InLat, const double InLon) : LatReference(InLat), LonReference(InLon), bIsValid(true) {};

	void SetReference(FLatLon LatLon)
	{
		SetReference(LatLon.Lat, LatLon.Lon);
	}

	void SetReference(const double InLat, const double InLon)
	{
		//if (bIsValid)
		//	return;

		LatReference = InLat;
		LonReference = InLon;
		bIsValid = true;

		UE_LOG(LogTemp, Warning, TEXT("Kulagin: FixReference: Lat = %f, Lon = %f"), LatReference, LonReference);
	}
};

USTRUCT(BlueprintType)
struct FLogPoint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) float Time;
	double Lat;
	double Lon;
	UPROPERTY(BlueprintReadWrite) float Alt;
	UPROPERTY(BlueprintReadWrite) float Roll;
	UPROPERTY(BlueprintReadWrite) float Pitch;
	UPROPERTY(BlueprintReadWrite) float Yaw;

	FLogPoint(float TimeIn = 0.f, double LatIn = 0., double LonIn = 0.,
			float AltIn = 0.f, float RollIn = 0.f, float PitchIn = 0.f, float YawIn = 0.f);
};

USTRUCT(BlueprintType)
struct FMapTileInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FLatLon TopLeft;
	UPROPERTY(BlueprintReadWrite) FLatLon TopRight;
	UPROPERTY(BlueprintReadWrite) FLatLon BottomLeft;
	UPROPERTY(BlueprintReadWrite) FLatLon BottomRight;
	UPROPERTY(BlueprintReadWrite) FLatLon Center;
	UPROPERTY(BlueprintReadWrite) int32 Zoom;
	UPROPERTY(BlueprintReadWrite) int32 TMSTileX;
	UPROPERTY(BlueprintReadWrite) int32 TMSTileY;

	FMapTileInfo(const double Lat, const double Lon, const int32 ZoomIn);

	FMapTileInfo(const FLatLon LatLon, const int32 ZoomIn) : FMapTileInfo(LatLon.Lat, LatLon.Lon, ZoomIn) {};

	FMapTileInfo() : FMapTileInfo(0., 0., 3) {};

	FIntPoint GetTMSTile() const { return FIntPoint(TMSTileX, TMSTileY); };

	void GetGoogleTile(int32 &GoogleTileX, int32 &GoogleTileY) const;

	FIntPoint GetGoogleTile() const
	{
		int32 GoogleTileX, GoogleTileY;
		GetGoogleTile(GoogleTileX, GoogleTileY);
		return FIntPoint(GoogleTileX, GoogleTileY);
	}

	void GetWorldSize(float &SizeX, float &SizeY) const;

	FString GetFilename() const 
	{
		int32 GoogleTileX, GoogleTileY;
		GetGoogleTile(GoogleTileX, GoogleTileY);
		return L"X_" + FString::FromInt(GoogleTileX) + L"_Y_" + FString::FromInt(GoogleTileY) + L"_Z_" + FString::FromInt(Zoom);
	}
};

UCLASS(BlueprintType)
class UMapperHeighmap : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite) float OffsetLeft;
	UPROPERTY(BlueprintReadWrite) float OffsetTop;
	UPROPERTY(BlueprintReadWrite) float OffsetRight;
	UPROPERTY(BlueprintReadWrite) float OffsetBottom;

	UPROPERTY(BlueprintReadWrite) int32 SizeX;

	UPROPERTY(BlueprintReadWrite) int32 SizeY;

	UPROPERTY(BlueprintReadWrite) int32 MinHeigh;

	UPROPERTY(BlueprintReadWrite) TArray<int32> Data;

	UFUNCTION(BlueprintCallable)
	void InitData(const int32 SizeXIn, const int32 SizeYIn);

	UFUNCTION(BlueprintPure)
	float GetHeigh(const float PercentageX, const float PercentageY, const int32 IndexX, const int32 IndexY, const int32 IndexCount);

	UMapperHeighmap();
};

USTRUCT(BlueprintType)
struct FMapTileImage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FMapTileInfo Info;
	UPROPERTY(BlueprintReadWrite) bool bBaseColorBeginLoad = false;
	UPROPERTY(BlueprintReadWrite) UTexture* BaseColor;
	UPROPERTY(BlueprintReadWrite) bool bHeighmapBeginLoad = false;
	UPROPERTY(BlueprintReadWrite) UMapperHeighmap* Heighmap;

	FMapTileImage(const FMapTileInfo& InfoIn, UTexture2DDynamic* BaseColorIn = nullptr, UMapperHeighmap* HeighmapIn = nullptr)
	: Info(InfoIn), BaseColor(BaseColorIn), Heighmap(HeighmapIn) {};

	FMapTileImage() : FMapTileImage(FMapTileInfo(), nullptr, nullptr) {};

	bool IsBaseColorValid() const { return BaseColor != nullptr; };

	bool IsHeighmapValid() const { return Heighmap != nullptr; };
};

USTRUCT(BlueprintType)
struct FMissionPointList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FBinaryFilePath FilePath;
	UPROPERTY(BlueprintReadWrite) TArray<FMissionPoint> Points;
	UPROPERTY(BlueprintReadWrite) FLatLon TopLeft;
	UPROPERTY(BlueprintReadWrite) FLatLon BottomRight;

	FMissionPointList(FBinaryFilePath FilePathIn, TArray<FMissionPoint> PointsIn, FLatLon TopLeftIn, FLatLon BottomRightIn)
		: FilePath(FilePathIn), Points(PointsIn), TopLeft(TopLeftIn), BottomRight(BottomRightIn) {};

	FMissionPointList() : FilePath(), Points(), TopLeft(), BottomRight() {};
};

USTRUCT(BlueprintType)
struct FPlaneMissionPointList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FBinaryFilePath FilePath;
	UPROPERTY(BlueprintReadWrite) TArray<FPlaneMissionPoint> Points;
	UPROPERTY(BlueprintReadWrite) FLatLon TopLeft;
	UPROPERTY(BlueprintReadWrite) FLatLon BottomRight;

	FPlaneMissionPointList(FBinaryFilePath FilePathIn, TArray<FPlaneMissionPoint> PointsIn, FLatLon TopLeftIn, FLatLon BottomRightIn)
		: FilePath(FilePathIn), Points(PointsIn), TopLeft(TopLeftIn), BottomRight(BottomRightIn) {};

	FPlaneMissionPointList() : FilePath(), Points(), TopLeft(), BottomRight() {};
};

USTRUCT(BlueprintType)
struct FLogPointList
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) FBinaryFilePath FilePath;
	UPROPERTY(BlueprintReadWrite) TArray<FLogPoint> Points;
	UPROPERTY(BlueprintReadWrite) FLatLon TopLeft;
	UPROPERTY(BlueprintReadWrite) FLatLon BottomRight;

	FLogPointList(FBinaryFilePath FilePathIn, TArray<FLogPoint> PointsIn, FLatLon TopLeftIn, FLatLon BottomRightIn)
		: FilePath(FilePathIn), Points(PointsIn), TopLeft(TopLeftIn), BottomRight(BottomRightIn) {};

	FLogPointList() : FilePath(), Points(), TopLeft(), BottomRight() {};
};

USTRUCT(BlueprintType)
struct FDangerZone
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite) float SecondsBegin = 0.f;
	UPROPERTY(BlueprintReadWrite) float SecondsOver = 0.f;
	UPROPERTY(BlueprintReadWrite) bool bIsCrossPath = 0.f;
	UPROPERTY(BlueprintReadWrite) bool bIsCrossScene = 0.f;

	FDangerZone(float BeginIn = 0.f, float OverIn = 0.f, bool bCrossPathIn = false, bool bCrossSceneIn = false) 
		: SecondsBegin(BeginIn), SecondsOver(OverIn), bIsCrossPath(bCrossPathIn), bIsCrossScene(bCrossSceneIn) {};
};

USTRUCT(BlueprintType)
struct FDroneData : public FTableRowBase
{
	GENERATED_BODY()

	/** Drone actor type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EDroneActorType ActorType;
	/** Drone skeletal mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMesh* SkeletalMesh;
	/** Drone static mesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMesh* StaticMesh;
	/** Drone class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Class;
	/** Max vertical speed in m/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxSpeedVertical;
	/** Max horizontal speed in m/s */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxSpeedHorizontal;

	FDroneData() : ActorType(EDroneActorType::DAT_Skeletal), SkeletalMesh(nullptr), StaticMesh(nullptr), Class(nullptr), MaxSpeedVertical(0.f), MaxSpeedHorizontal(0.f) {};

	bool IsValid() const;
	
};

USTRUCT(BlueprintType)
struct FMapperLevelData : public FTableRowBase
{
	GENERATED_BODY()

	/*
	 * Row name is level name in main menu
	 * This Level Name is actual name for level to open 
	 * May row name and this Level Name may be same
	 */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite) FName LevelName;
	/** Start Point for simulation mode if map dont already have one */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector StartPoint;
	/** For orientation. Lat/Lon for first and second points in WGS84 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FVector2D> LatLon;
	/** For orientation. Location for first and second points in unreal units */
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FVector> Locations;

	FMapperLevelData() : /*LevelName(NAME_None), */StartPoint(FVector::ZeroVector), LatLon({ FVector2D::ZeroVector, FVector2D::ZeroVector }), Locations({ FVector::ZeroVector, FVector::ZeroVector }) {};

};