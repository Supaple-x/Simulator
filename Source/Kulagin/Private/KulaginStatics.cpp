// Fill out your copyright notice in the Description page of Project Settings.

#include "KulaginStatics.h"

#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

#include "WGS84ToCartesian.hpp"
#include "GlobalMercator.hpp"
#include "MapperGameMode.h"
#include "MapperGameInstance.h"
#include "MapperTileComponent.h"

#include <fstream>
#include <string>
#include <cwctype>

FString UKulaginStatics::LastPath = "";

FReferenceLatLon UKulaginStatics::Reference = FReferenceLatLon();

double UKulaginStatics::Scale = 10.;

int32 UKulaginStatics::MaxZoom = 19;

int32 UKulaginStatics::MinZoom = 16;

float UKulaginStatics::PixelsPerSecond = 20;

float UKulaginStatics::MaxSnappingSeconds = 0.5f;

FString UKulaginStatics::GetCurrentTime(int32& Year, int32& Month, int32& Day, int32& Hours, int32& Minutes, int32& Seconds)
{
	const FDateTime Now = FDateTime::Now();
	
	Year =		Now.GetYear();
	Month =		Now.GetMonth();
	Day =		Now.GetDay();
	Hours =		Now.GetHour();
	Minutes =	Now.GetMinute();
	Seconds =	Now.GetSecond();

	const TArray<FStringFormatArg> Args = {Year, FixSingleInt(Month), FixSingleInt(Day), FixSingleInt(Hours), FixSingleInt(Minutes), FixSingleInt(Seconds)};
	const FString Result = FString::Format(L"{0}-{1}-{2}_{3}-{4}-{5}", Args);

	return Result;
}

bool UKulaginStatics::SetOrientActorsFromFile(UPARAM(ref) TArray<FMissionPoint> &Points, TArray<AActor*> NaviPoints, UTexture2DDynamic* Texture, TArray<USceneComponent*> ImageCorners)
{
	if (Texture == nullptr || ImageCorners.Num() != 4 || Points.Num() != 2 || NaviPoints.Num() != 2)
		return false;

	for (const USceneComponent* Corner : ImageCorners)
	{
		if (Corner == nullptr)
			return false;
	}

	for (const AActor* NaviPoint : NaviPoints)
	{
		if (NaviPoint == nullptr)
			return false;
	}

	FBinaryFilePath Path;

	if (AskUserForFile(Path, L"Select orient file", "Orient file|*.tab;*.@@@") == false)
		return false;

	TArray<double> Lat, Lon;
	TArray<int32> ImageX, ImageY;

	if (GetOrientValuesFromFile(Path, Lat, Lon, ImageX, ImageY) == false)
		return false;

	Points[0].TargetLatLon.Lat = Lat[0];
	Points[0].TargetLatLon.Lon = Lon[0];
	Points[1].TargetLatLon.Lat = Lat[1];
	Points[1].TargetLatLon.Lon = Lon[1];
	
	const FVector TopLeft = ImageCorners[0]->GetComponentLocation();
	const FVector TopRight = ImageCorners[1]->GetComponentLocation();
	const FVector BottomLeft = ImageCorners[2]->GetComponentLocation();
	const FVector BottomRight = ImageCorners[3]->GetComponentLocation();

	const int32 SizeX = Texture->SizeX;
	const int32 SizeY = Texture->SizeY;

	const float WorldSizeX = (TopRight - TopLeft).Size();
	const float WorldSizeY = (BottomLeft - TopLeft).Size();

	const FVector LeftToRight = (TopRight - TopLeft).GetSafeNormal();
	const FVector TopToBottom = (BottomLeft - TopLeft).GetSafeNormal();

	const float TargetXFirst = (float(ImageX[0]) / float(SizeX)) * WorldSizeX;
	const float TargetYFirst = (float(ImageY[0]) / float(SizeY)) * WorldSizeY;
	const FVector TargetLocFirst = TopLeft + TargetXFirst * LeftToRight + TargetYFirst * TopToBottom;

	const float TargetXSecond = (float(ImageX[1]) / float(SizeX)) * WorldSizeX;
	const float TargetYSecond = (float(ImageY[1]) / float(SizeY)) * WorldSizeY;
	const FVector TargetLocSecond = TopLeft + TargetXSecond * LeftToRight + TargetYSecond * TopToBottom;

	NaviPoints[0]->SetActorLocation(TargetLocFirst);
	NaviPoints[1]->SetActorLocation(TargetLocSecond);

	return true;
}

bool UKulaginStatics::GetOrientValuesFromFile(FBinaryFilePath Path, TArray<double> &Lat, TArray<double> &Lon, TArray<int32> &ImageX, TArray<int32> &ImageY)
{
	TArray<double> TempLat, TempLon;
	TArray<int32> TempImageX, TempImageY;

	if (Path.Extension == L"tab")
	{
		if (GetOrientValueFromFileTab(Path, TempLat, TempLon, TempImageX, TempImageY) == false)
			return false;
	}
	else if (Path.Extension == L"@@@")
	{
		if (GetOrientValueFromFileAtAtAt(Path, TempLat, TempLon, TempImageX, TempImageY) == false)
			return false;
	}
	else 
		return false;

	const TArray<int32> Lengths = TArray<int32>{ TempLat.Num(), TempLon.Num(), TempImageX.Num(), TempImageY.Num() };
	if (FMath::Min(Lengths) != FMath::Max(Lengths))
		return false;
	const int32 Length = Lengths[0];

	if (Length < 2)
		return false;
	else if (Length == 2)
	{
		Lat = TempLat;
		Lon = TempLon;
		ImageX = TempImageX;
		ImageY = TempImageY;
		return true;
	}

	int32 MinDistance = 65536;
	int32 MinIndexFirst = 0;
	int32 MinIndexSecond = 0;

	for (int32 i = 0; i < Length; i++)
	{
		const FVector2D FirstPoint = FVector2D(TempImageX[i], TempImageY[i]);
		for (int32 j = 0; j < Length; j++)
		{
			const FVector2D SecondPoint = FVector2D(TempImageX[j], TempImageY[j]);
			const float CurrentDistance = FVector2D::Distance(FirstPoint, SecondPoint);
			if (j != i && CurrentDistance < MinDistance)
			{
				MinDistance = CurrentDistance;
				MinIndexFirst = i;
				MinIndexSecond = j;
			}
		}
	}

	Lat = { TempLat[MinIndexFirst], TempLat[MinIndexSecond] };
	Lon = { TempLon[MinIndexFirst], TempLon[MinIndexSecond] };
	ImageX = { TempImageX[MinIndexFirst], TempImageX[MinIndexSecond] };
	ImageY = { TempImageY[MinIndexFirst], TempImageY[MinIndexSecond] };
	return true;
}		

bool UKulaginStatics::GetOrientValueFromFileTab(FBinaryFilePath Path, TArray<double> &Lat, TArray<double> &Lon, TArray<int32> &ImageX, TArray<int32> &ImageY)
{
	const FString FullPath = Path.GetFullPath();
	const TCHAR* WideNativePath = *FullPath;

	std::fstream FileStream;

	FileStream.open(WideNativePath, std::fstream::in);

	if (FileStream.is_open() == false)
		return false;

	std::string s;
	int i = 0;
	bool Ready = false;
	while (getline(FileStream, s) && i < 4)
	{
		if (s.find("Type") == 0)
		{
			Ready = true;
			continue;
		}
		if (!Ready)
			continue;

		if (s.find("Label") == std::string::npos)
			break;

		// (38.714977,45.090044) (0,0) Label "Pt 1"
		// "*(%d,%d) (%i,%i)*"
		// 0(234.

		const size_t LonStart = s.find('(') + 1;
		const size_t LonEnd = s.find(',') - 1;

		const size_t LatStart = s.find(',') + 1;
		const size_t LatEnd = s.find(')') - 1;

		const size_t XStart = s.find('(', LonEnd + 2) + 1;
		const size_t XEnd = s.find(',', LonEnd + 2) - 1;

		const size_t YStart = s.find(',', LonEnd + 2) + 1;
		const size_t YEnd = s.find(')', LonEnd + 2) - 1;

		const std::string LatString = s.substr(LatStart, LatEnd - LatStart + 1);
		const std::string LonString = s.substr(LonStart, LonEnd - LonStart + 1);
		const std::string XString = s.substr(XStart, XEnd - XStart + 1);
		const std::string YString = s.substr(YStart, YEnd - YStart + 1);

		const double CurrentLat = stod(LatString);
		const double CurrentLon = stod(LonString);
		const int CurrentX = stoi(XString);
		const int CurrentY = stoi(YString);

		Lat.Add(CurrentLat);
		Lon.Add(CurrentLon);
		ImageX.Add(round(CurrentX));
		ImageY.Add(round(CurrentY));

		i++;
	}

	FileStream.close();

	return false;
}

bool UKulaginStatics::GetOrientValueFromFileAtAtAt(FBinaryFilePath Path, TArray<double> &Lat, TArray<double> &Lon, TArray<int32> &ImageX, TArray<int32> &ImageY)
{
	const FString FullPath = Path.GetFullPath();
	const TCHAR* WideNativePath = *FullPath;

	std::fstream FileStream;

	FileStream.open(WideNativePath, std::fstream::in);

	if (FileStream.is_open() == false)
		return false;

	std::string s;
	int i = 0;
	while (std::getline(FileStream, s) && i < 4)
	{
		if (s.find('#') != std::string::npos)
		{
			continue;
		}

		double CurrentLat = 0., CurrentLon = 0., CurrentX = 0., CurrentY = 0.;

		s += ' ';
		std::string Temp = "";
		int Counter = 1;
		for (const char c : s)
		{
			if (c == '.' || isdigit(c))
			{
				Temp += c;
			}
			else if (Temp.length() > 0)
			{
				switch (Counter)
				{
				case 1: CurrentX = stod(Temp); break;
				case 2: CurrentY = stod(Temp); break;
				case 3: CurrentLat = stod(Temp); break;
				case 4: CurrentLon = stod(Temp);
				}
				Temp = "";
				Counter++;
			}
		}

		Lat.Add(CurrentLat);
		Lon.Add(CurrentLon);
		ImageX.Add(round(CurrentX));
		ImageY.Add(round(CurrentY));

		i++;
	}

	FileStream.close();

	return true;
}

AMapperGameMode* UKulaginStatics::GetMapperGameMode(const UObject* WorldContextObject)
{
	return Cast<AMapperGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
};

UMapperGameInstance* UKulaginStatics::GetMapperGameInstance(const UObject* WorldContextObject)
{
	return Cast<UMapperGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
};

FMapTileInfo UKulaginStatics::GetMapTile(FLatLon LatLon, int32 Zoom)
{
	/*FBinaryFilePath Path;
	if (AskUserForFile(Path, L"Select files", L"Mission files|*.bin|Log files|*.txt") == false)
		return FMapTileInfo();

	double Lat, Lon;
	if (Path.Extension == "bin")
	{
		const TArray<FMissionPoint> Points = GetPointsFromFile(Path);
		if (Points.IsValidIndex(0))
			return FMapTileInfo();
		const FMissionPoint &CurrentPoint = Points[0];
		Lat = CurrentPoint.TargetLatLon.Lat;
		Lon = CurrentPoint.TargetLatLon.Lon;
	} 
	else
	{
		const TArray<FLogPoint> Points = GetLogPointsFromFile(Path);
		if (Points.IsValidIndex(0))
			return FMapTileInfo();
		const FLogPoint &CurrentPoint = Points[0];
		Lat = CurrentPoint.Lat;
		Lon = CurrentPoint.Lon;
	}*/

	return FMapTileInfo(LatLon.Lat, LatLon.Lon, Zoom);
}

FLatLon UKulaginStatics::LatLonTopLeft(FLatLon A, FLatLon B)
{
	const double MaxLat = FMath::Max<double>(A.Lat, B.Lat);
	const double MinLon = FMath::Min<double>(A.Lon, B.Lon);
	return FLatLon(MaxLat, MinLon);
}

FLatLon UKulaginStatics::LatLonBottomRight(FLatLon A, FLatLon B)
{
	const double MinLat = FMath::Min<double>(A.Lat, B.Lat);
	const double MaxLon = FMath::Max<double>(A.Lon, B.Lon);
	return FLatLon(MinLat, MaxLon);
}

FLatLon UKulaginStatics::LatLonCenter(FLatLon A, FLatLon B)
{
	const double MaxLat = FMath::Max<double>(A.Lat, B.Lat);
	const double MaxLon = FMath::Max<double>(A.Lon, B.Lon);
	const double MinLat = FMath::Min<double>(A.Lat, B.Lat);
	const double MinLon = FMath::Min<double>(A.Lon, B.Lon);
	const double CenterLat = MinLat + (MaxLat - MinLat) * 0.5;
	const double CenterLon = MinLon + (MaxLon - MinLon) * 0.5;
	return FLatLon(CenterLat, CenterLon);
}

UMapperTileComponent* UKulaginStatics::SpawnMapTile(USceneComponent* Parent, const FIntPoint TileIndex, const FVector RelativeLocation)
{
	if (UKulaginStatics::GetMapperGameMode(Parent) == nullptr || UKulaginStatics::GetMapperGameMode(Parent)->MaxZoomImages.Contains(TileIndex))
		return nullptr;

	UMapperTileComponent* Temp = NewObject<UMapperTileComponent>(Parent, UMapperTileComponent::StaticClass());
	if (Temp)
	{
		UKulaginStatics::GetMapperGameMode(Parent)->MaxZoomImages.Add(TileIndex);

		Temp->SetupAttachment(Parent);
		Temp->SetRelativeLocation(RelativeLocation);
		Temp->SetRelativeRotation(FRotator::ZeroRotator);
		Temp->SetRelativeScale3D(FVector(1.005f));
		//Temp->SetStaticMesh(Mesh);
		Temp->RegisterComponent();
		//Temp->UpdateTile();
		//Temp->GetMapTileMinZoom();
	}
	return Temp;
}

void UKulaginStatics::SpawnDefaultTiles(USceneComponent* Parent, FLatLon TopLeft, FLatLon BottomRight)
{
#if WITH_EDITOR
	AMapperGameMode* GM = UKulaginStatics::GetMapperGameMode(Parent);
	if (GM && GM->bEnableDefaultTiles == false) return;
#endif
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: SpawnDefaultTiles v17"));

	const FMapTileInfo InfoTopLeft(TopLeft, MaxZoom);
	const FMapTileInfo InfoBottomRight(BottomRight, MaxZoom);
	const FMapTileInfo InfoCenter(LatLonCenter(TopLeft, BottomRight), MaxZoom);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: SpawnDefaultTiles:  TopLeft Lat %f, Lon %f"), InfoTopLeft.Center.Lat, InfoTopLeft.Center.Lon);
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: SpawnDefaultTiles: BotRight Lat %f, Lon %f"), InfoBottomRight.Center.Lat, InfoBottomRight.Center.Lon);
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: SpawnDefaultTiles:   Center Lat %f, Lon %f"), InfoCenter.Center.Lat, InfoCenter.Center.Lon);

	const FIntPoint TileTopLeft = InfoTopLeft.GetGoogleTile();
	const FIntPoint TileBottomRight = InfoBottomRight.GetGoogleTile();
	const FIntPoint TileCenter = InfoCenter.GetGoogleTile();

#if WITH_EDITOR
	const int32 TilesMin = GM ? GM->DefaultTilesCount.X : 8;
	const int32 TilesMax = GM ? GM->DefaultTilesCount.Y : 16;
#else
	const int32 TilesMin = 8;
	const int32 TilesMax = 16;
#endif

	const int32 TilesCountX = FMath::Clamp(FMath::Abs(TileTopLeft.X - TileBottomRight.X), TilesMin, TilesMax);
	const int32 TilesCountY = FMath::Clamp(FMath::Abs(TileTopLeft.Y - TileBottomRight.Y), TilesMin, TilesMax);

	const int32 XCenter = TileCenter.X;
	const int32 YCenter = TileCenter.Y;

	const int32 TilesRadiusX = TilesCountX * 0.5;
	const int32 TilesRadiusY = TilesCountY * 0.5;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: SpawnDefaultTiles: TilesRadiusX = %i, TilesRadiusY = %i"), TilesRadiusX, TilesRadiusY);

	for (int32 IndexX = XCenter - TilesRadiusX; IndexX <= XCenter + TilesRadiusX; IndexX++)
	{
		const FVector OffsetX = FVector(100.f, 0.f, 0.f) * (IndexX - XCenter);
		for (int32 IndexY = YCenter - TilesRadiusX; IndexY <= YCenter + TilesRadiusY; IndexY++)
		{
			const FVector OffsetY = FVector(0.f, 100.f, 0.f) * (IndexY - YCenter);
			UMapperTileComponent* CurrentTile = SpawnMapTile(Parent, FIntPoint(IndexX, IndexY), OffsetX + OffsetY);
			if (CurrentTile)
			{	
				CurrentTile->GetMapTileMaxZoom();
			}
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: SpawnDefaultTiles DONE"));
}

void UKulaginStatics::GetMinZoomIndex(const FMapTileInfo &Info, const FVector TileCenter, int32 &IndexX, int32 &IndexY)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: GetMinZoomIndex: Lat = %f, Lon = %f"), Info.TopLeft.Lat, Info.TopLeft.Lon);
	const FVector LocTopLeft = LatLonToUE(Info.TopLeft, 0.f);
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: GetMinZoomIndex: LocTopLeft = %s"), *LocTopLeft.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: GetMinZoomIndex: TileCenter = %s"), *TileCenter.ToString());
	const float DistX = FMath::Abs(LocTopLeft.X - TileCenter.X);
	const float DistY = FMath::Abs(LocTopLeft.Y - TileCenter.Y);
	float SizeX, SizeY;
	Info.GetWorldSize(SizeX, SizeY);
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: GetMinZoomIndex: DistX = %i, DistY = %i, SizeX = %i, SizeX = %i,"), int32(DistX), int32(DistY), int32(SizeX), int32(SizeY));
	const int32 IndexMax = GetZoomMaxIndex();
	IndexY = FMath::FloorToInt(DistX / SizeX * float(IndexMax));
	IndexX = FMath::FloorToInt(DistY / SizeY * float(IndexMax));
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: GetMinZoomIndex: IndexX = %i, IndexY = %i"), IndexX, IndexY);
}

bool UKulaginStatics::AskUserForFile(FBinaryFilePath &Path, FString DialogTitle, FString FileTypes)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
		return false;

	if (LastPath.IsEmpty())
#if WITH_EDITOR
		LastPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
#else
		LastPath = FPaths::ConvertRelativePathToFull(FPaths::RootDir());
#endif

	TArray<FString> OutFilenames;
	if (!DesktopPlatform->OpenFileDialog(nullptr, DialogTitle, LastPath, L"", FileTypes, 0x00, OutFilenames))
		return false;

	if (OutFilenames.Num() != 1)
		return false;

	Path = FBinaryFilePath(OutFilenames[0]);

	LastPath = Path.GetPathOnly();

	return true;
}

TArray<FBinaryFilePath> UKulaginStatics::AskUserForFiles(FString DialogTitle, FString FileTypes)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
		return {};

	if (LastPath.IsEmpty())
#if WITH_EDITOR
		LastPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
#else
		LastPath = FPaths::ConvertRelativePathToFull(FPaths::RootDir());
#endif

	TArray<FString> OutFilenames;
	if (!DesktopPlatform->OpenFileDialog(nullptr, DialogTitle, LastPath, L"", FileTypes, 0x01, OutFilenames))
		return {};

	if (OutFilenames.Num() <= 0)
		return {};

	TArray<FBinaryFilePath> BinaryFiles;
	for (FString &CurrentFullPath : OutFilenames)
	{
		BinaryFiles.Add(FBinaryFilePath(CurrentFullPath));
	}

	LastPath = BinaryFiles.Last().GetPathOnly();

	return BinaryFiles;
}

FString UKulaginStatics::AskUserForDirectory()
{
	return "";
}

FLogPointList UKulaginStatics::GetLogPointsFromFile(FBinaryFilePath Path)
{
	const FString FullPath = Path.GetFullPath();
	const TCHAR* WideNativePath = *FullPath;

	std::fstream FileStream;

	FileStream.open(WideNativePath, std::fstream::in);

	if (FileStream.is_open() == false)
		return {};

	TArray<FLogPoint> Temp;

	std::string s = "";

	double MaxLat = -95., MaxLon = -185., MinLat = 95., MinLon = 185.;

	int i = 0;
	bool Ready = false;
	while (getline(FileStream, s) && i < 50000)
	{
		if (s.find(";sau") != std::string::npos || s.length() <= 1)
			continue;
		i++;

		// Parse begin

		//UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: GetLogPointsFromFile: s = '%s'"), *FString(s.c_str()));

		s += " ";

		TArray<std::string> CurrentRow;
		CurrentRow.Init("", 7);

		int32 CeilIndex = 0;
		bool bFirstSpace = false;
		for (const char c : s)
		{
			if (CeilIndex >= 7) break;
			if (c == ' ' || c == '\t')
			{
				if (bFirstSpace)
				{
					bFirstSpace = false;
					if (CurrentRow[CeilIndex].length() <= 0)
					{
						//UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: GetLogPointsFromFile: CurrentRow[CeilIndex].length() <= 0"));
						break;
					}
					CeilIndex++;
				}
			}
			else
			{
				bFirstSpace = true;
				CurrentRow[CeilIndex] += c;
			}
		}
		if (CeilIndex < 7) continue;
		//for (int32 j = 0; j < 7; j++)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Kulagin: Statics: GetLogPointsFromFile: j = %i, Ceil = '%s'"), j, *FString(CurrentRow[j].c_str()));
		//}

		const std::string StringHours = CurrentRow[0].substr(0, 2);
		const std::string StringMinutes = CurrentRow[0].substr(3, 2);
		const std::string StringSeconds = CurrentRow[0].substr(6, 6);

		const std::string StringLat = CurrentRow[1].substr(0, 2) + '.' + CurrentRow[1].substr(3, 2) + CurrentRow[1].substr(6, 2) + CurrentRow[1].substr(9, 2);
		const std::string StringLon = CurrentRow[2].substr(0, 2) + '.' + CurrentRow[2].substr(3, 2) + CurrentRow[2].substr(6, 2) + CurrentRow[2].substr(9, 2);

		// Parse end

		// 00:00:00.000	45�13'51.1209"N	38�19'19.7438"E	18,494	-0,715	-1,714	328,812
		// 012345

		//const std::string StringHours = s.substr(0, 2);
		//const std::string StringMinutes = s.substr(3, 2);
		//const std::string StringSeconds = s.substr(6, 6);
		//const std::string StringLat = s.substr(13, 2) + '.' + s.substr(16, 2) + s.substr(19, 2) + s.substr(22, 4);
		//const std::string StringLon = s.substr(29, 2) + '.' + s.substr(32, 2) + s.substr(35, 2) + s.substr(38, 4);
		//const std::string Remain = s.substr(45, s.length() - 45);
		std::string StringAlt = CurrentRow[3];
		std::string StringRoll = CurrentRow[4];
		std::string StringPitch = CurrentRow[5];
		std::string StringYaw = CurrentRow[6];
		/*int ParseIndex = 1;
		for (const char c : Remain)
		{
			std::string *CurrentString = &StringAlt;
			switch (ParseIndex)
			{
			case 2: CurrentString = &StringRoll; break; case 3: CurrentString = &StringPitch; break;
			case 4: CurrentString = &StringYaw;
			}
			if (c == ',' || c == '.')
				(*CurrentString) += '.';
			else if (c == '-' || (c >= '0' && c <= '9'))
				(*CurrentString) += c;
			else
				ParseIndex++;
		}*/

		const double Hours = stod(StringHours);
		const double Minutes = stod(StringMinutes);
		const double Seconds = stod(StringSeconds);
		const double Time = Hours * 3600. + Minutes * 60. + Seconds;

		const double Lat = stod(StringLat);
		const double Lon = stod(StringLon);
		const double Alt = stod(StringAlt);

		const double Roll = stod(StringRoll);
		const double Pitch = stod(StringPitch);
		const double Yaw = stod(StringYaw);

		Temp.Add(FLogPoint(Time, Lat, Lon, Alt, Roll, Pitch, Yaw));

		MaxLat = FMath::Max<double>(MaxLat, Lat);
		MaxLon = FMath::Max<double>(MaxLon, Lon);
		MinLat = FMath::Min<double>(MinLat, Lat);
		MinLon = FMath::Min<double>(MinLon, Lon);
	}

	const FLatLon TopLeft(MaxLat, MinLon);
	const FLatLon BottomRight(MinLat, MaxLon);

	FileStream.close();

	return FLogPointList(Path, Temp, TopLeft, BottomRight);
}

FMissionPointList UKulaginStatics::GetPointsFromFile(FBinaryFilePath Path)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetPointsFromFile: Path = %s"), *Path.GetFullPath());

	const FString FullPath = Path.GetFullPath();
	const TCHAR* WideNativePath = *FullPath;
	// const char* NativePath = TCHAR_TO_ANSI(WideNativePath);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetPointsFromFile: Native Path via FString = %s"), *FString(WideNativePath));

	FLatLon TopLeft, BottomRight;
	TArray<FMissionPointNative> PointsNative = GetPointsNativeFromFile(WideNativePath, TopLeft, BottomRight);

	return FMissionPointList(Path, NativePointsToPoints(PointsNative), TopLeft, BottomRight);
}

TArray<FMissionPointNative> UKulaginStatics::GetPointsNativeFromFile(const TCHAR* Path, FLatLon &TopLeft, FLatLon &BottomRight)
{
	std::fstream FileStream;

	FileStream.open(Path, std::fstream::in | std::fstream::binary);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetPointsNativeFromFile: FileStream %s"), FileStream.is_open() ? *FString("OPENED") : *FString("CLOSED"));

	if (FileStream.is_open() == false)
		return {};

	FileStream.seekg(0, FileStream.end);
	int32 Length = FileStream.tellg();
	FileStream.seekg(0, FileStream.beg);

	FMissionPointNative BlankPoint;

	int32 PointsCount = Length / BlankPoint.GetSize();

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetPointsNativeFromFile: PointsCount = %i"), PointsCount);

	if (PointsCount <= 0)
		return {};

	TArray<FMissionPointNative> Points;

	double MaxLat = -95., MaxLon = -185., MinLat = 95., MinLon = 185.;

	for (int32 i = 0; i < PointsCount; i++)
	{
		FileStream.read((char*)&BlankPoint.targetLat, sizeof(BlankPoint.targetLat));
		FileStream.read((char*)&BlankPoint.targetLon, sizeof(BlankPoint.targetLon));
		FileStream.read((char*)&BlankPoint.targetAlt, sizeof(BlankPoint.targetAlt));
		FileStream.read((char*)&BlankPoint.targetRadius, sizeof(BlankPoint.targetRadius));
		FileStream.read((char*)&BlankPoint.loiterTime, sizeof(BlankPoint.loiterTime));
		FileStream.read((char*)&BlankPoint.maxHorizSpeed, sizeof(BlankPoint.maxHorizSpeed));
		FileStream.read((char*)&BlankPoint.maxVertSpeed, sizeof(BlankPoint.maxVertSpeed));
		FileStream.read((char*)&BlankPoint.poiLat, sizeof(BlankPoint.poiLat));
		FileStream.read((char*)&BlankPoint.poiLon, sizeof(BlankPoint.poiLon));
		FileStream.read((char*)&BlankPoint.poiHeading, sizeof(BlankPoint.poiHeading));
		FileStream.read((char*)&BlankPoint.poiAltitude, sizeof(BlankPoint.poiAltitude));
		FileStream.read((char*)&BlankPoint.flags, sizeof(BlankPoint.flags));
		FileStream.read((char*)&BlankPoint.photo, sizeof(BlankPoint.photo));
		FileStream.read((char*)&BlankPoint.panoSectorsCount, sizeof(BlankPoint.panoSectorsCount));
		FileStream.read((char*)&BlankPoint.panoDeltaAngle, sizeof(BlankPoint.panoDeltaAngle));
		FileStream.read((char*)&BlankPoint.poiPitch, sizeof(BlankPoint.poiPitch));
		FileStream.read((char*)&BlankPoint.poiRoll, sizeof(BlankPoint.poiRoll));
		FileStream.read((char*)&BlankPoint.type, sizeof(BlankPoint.type));

		MaxLat = FMath::Max<double>(MaxLat, BlankPoint.targetLat);
		MaxLon = FMath::Max<double>(MaxLon, BlankPoint.targetLon);
		MinLat = FMath::Min<double>(MinLat, BlankPoint.targetLat);
		MinLon = FMath::Min<double>(MinLon, BlankPoint.targetLon);

		Points.Add(BlankPoint);
	}

	FileStream.close();

	TopLeft.Lat = MaxLat;
	TopLeft.Lon = MinLon;

	BottomRight.Lat = MinLat;
	BottomRight.Lon = MaxLon;

	return Points;
}

FPlaneMissionPointList UKulaginStatics::GetPlanePointsFromFile(FBinaryFilePath Path)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetPlanePointsFromFile: Path = %s"), *Path.GetFullPath());

	const FString FullPath = Path.GetFullPath();
	const TCHAR* WideNativePath = *FullPath;
	// const char* NativePath = TCHAR_TO_ANSI(WideNativePath);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetPlanePointsFromFile: Native Path via FString = %s"), *FString(WideNativePath));

	FLatLon TopLeft, BottomRight;
	TArray<FPlaneMissionPointNative> PointsNative = GetPlanePointsNativeFromFile(WideNativePath, TopLeft, BottomRight);

	return FPlaneMissionPointList(Path, PlaneNativePointsToPoints(PointsNative), TopLeft, BottomRight);
}

TArray<FPlaneMissionPointNative> UKulaginStatics::GetPlanePointsNativeFromFile(const TCHAR* Path, FLatLon &TopLeft, FLatLon &BottomRight)
{
	std::fstream FileStream;

	FileStream.open(Path, std::fstream::in | std::fstream::binary);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetPlanePointsNativeFromFile: FileStream %s"), FileStream.is_open() ? *FString("OPENED") : *FString("CLOSED"));

	if (FileStream.is_open() == false)
		return {};

	FileStream.seekg(0, FileStream.end);
	int32 Length = FileStream.tellg();
	FileStream.seekg(0, FileStream.beg);

	FPlaneMissionPointNative BlankPoint;

	int32 PointsCount = Length / BlankPoint.GetSize();

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: GetPlanePointsNativeFromFile: PointsCount = %i"), PointsCount);

	if (PointsCount <= 0)
		return {};

	TArray<FPlaneMissionPointNative> Points;

	double MaxLat = -95., MaxLon = -185., MinLat = 95., MinLon = 185.;

	for (int32 i = 0; i < PointsCount; i++)
	{
		FileStream.read((char*)&BlankPoint.Lat, sizeof(BlankPoint.Lat));
		FileStream.read((char*)&BlankPoint.Lon, sizeof(BlankPoint.Lon));
		FileStream.read((char*)&BlankPoint.Alt, sizeof(BlankPoint.Alt));
		FileStream.read((char*)&BlankPoint.Time, sizeof(BlankPoint.Time));
		
		MaxLat = FMath::Max<double>(MaxLat, BlankPoint.Lat);
		MaxLon = FMath::Max<double>(MaxLon, BlankPoint.Lon);
		MinLat = FMath::Min<double>(MinLat, BlankPoint.Lat);
		MinLon = FMath::Min<double>(MinLon, BlankPoint.Lon);

		Points.Add(BlankPoint);
	}

	FileStream.close();

	TopLeft.Lat = MaxLat;
	TopLeft.Lon = MinLon;

	BottomRight.Lat = MinLat;
	BottomRight.Lon = MaxLon;

	return Points;
}

TArray<FMissionPoint> UKulaginStatics::NativePointsToPoints(TArray<FMissionPointNative> PointsNative)
{
	TArray<FMissionPoint> TempPoints;
	for (FMissionPointNative CurrentPointNative : PointsNative)
	{
		TempPoints.Add(FMissionPoint(CurrentPointNative));
	}
	return TempPoints;
}

TArray<FPlaneMissionPoint> UKulaginStatics::PlaneNativePointsToPoints(TArray<FPlaneMissionPointNative> PointsNative)
{
	TArray<FPlaneMissionPoint> TempPoints;
	for (FPlaneMissionPointNative CurrentPointNative : PointsNative)
	{
		TempPoints.Add(FPlaneMissionPoint(CurrentPointNative));
	}
	return TempPoints;
}

bool UKulaginStatics::SavePointsToFile(FBinaryFilePath Path, TArray<FMissionPoint> Points)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: SavePointsToFile: Path = %s"), *Path.GetFullPath());

	if (Path.PathOnly.IsEmpty())
#if WITH_EDITOR
		Path.PathOnly = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
#else
		Path.PathOnly = FPaths::ConvertRelativePathToFull(FPaths::RootDir());
#endif

	const FString FullPath = Path.GetFullPath();
	const TCHAR* WideNativePath = *FullPath;
	//const char* NativePath = TCHAR_TO_ANSI(WideNativePath);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: SavePointsToFile: Native Path via FString = %s"), *FString(WideNativePath));

	const bool Result = SavePointsNativeToFile(WideNativePath, PointsToNativePoints(Points));

	//IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	return Result;
}

bool UKulaginStatics::SavePointsNativeToFile(const TCHAR* Path, TArray<FMissionPointNative> PointsNative)
{
	std::fstream FileStream;

	FileStream.open(Path, std::fstream::out | std::fstream::binary);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: SavePointsNativeToFile: FileStream %s"), FileStream.is_open() ? *FString("OPENED") : *FString("CLOSED"));

	if (!FileStream.is_open())
		return false;

	for (FMissionPointNative CurrentNativePoint : PointsNative)
	{
		FileStream.write((char*)&CurrentNativePoint.targetLat, sizeof(CurrentNativePoint.targetLat));
		FileStream.write((char*)&CurrentNativePoint.targetLon, sizeof(CurrentNativePoint.targetLon));
		FileStream.write((char*)&CurrentNativePoint.targetAlt, sizeof(CurrentNativePoint.targetAlt));
		FileStream.write((char*)&CurrentNativePoint.targetRadius, sizeof(CurrentNativePoint.targetRadius));
		FileStream.write((char*)&CurrentNativePoint.loiterTime, sizeof(CurrentNativePoint.loiterTime));
		FileStream.write((char*)&CurrentNativePoint.maxHorizSpeed, sizeof(CurrentNativePoint.maxHorizSpeed));
		FileStream.write((char*)&CurrentNativePoint.maxVertSpeed, sizeof(CurrentNativePoint.maxVertSpeed));
		FileStream.write((char*)&CurrentNativePoint.poiLat, sizeof(CurrentNativePoint.poiLat));
		FileStream.write((char*)&CurrentNativePoint.poiLon, sizeof(CurrentNativePoint.poiLon));
		FileStream.write((char*)&CurrentNativePoint.poiHeading, sizeof(CurrentNativePoint.poiHeading));
		FileStream.write((char*)&CurrentNativePoint.poiAltitude, sizeof(CurrentNativePoint.poiAltitude));
		FileStream.write((char*)&CurrentNativePoint.flags, sizeof(CurrentNativePoint.flags));
		FileStream.write((char*)&CurrentNativePoint.photo, sizeof(CurrentNativePoint.photo));
		FileStream.write((char*)&CurrentNativePoint.panoSectorsCount, sizeof(CurrentNativePoint.panoSectorsCount));
		FileStream.write((char*)&CurrentNativePoint.panoDeltaAngle, sizeof(CurrentNativePoint.panoDeltaAngle));
		FileStream.write((char*)&CurrentNativePoint.poiPitch, sizeof(CurrentNativePoint.poiPitch));
		FileStream.write((char*)&CurrentNativePoint.poiRoll, sizeof(CurrentNativePoint.poiRoll));
		FileStream.write((char*)&CurrentNativePoint.type, sizeof(CurrentNativePoint.type));
	}

	FileStream.close();

	return true;
}

bool UKulaginStatics::SavePlanePointsToFile(FBinaryFilePath Path, TArray<FPlaneMissionPoint> Points)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: SavePlanePointsToFile: Path = %s"), *Path.GetFullPath());

	if (Path.PathOnly.IsEmpty())
#if WITH_EDITOR
		Path.PathOnly = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
#else
		Path.PathOnly = FPaths::ConvertRelativePathToFull(FPaths::RootDir());
#endif

	const FString FullPath = Path.GetFullPath();
	const TCHAR* WideNativePath = *FullPath;
	//const char* NativePath = TCHAR_TO_ANSI(WideNativePath);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: SavePlanePointsToFile: Native Path via FString = %s"), *FString(WideNativePath));

	const bool Result = SavePlanePointsNativeToFile(WideNativePath, PlanePointsToNativePoints(Points));

	//IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	return Result;
}

bool UKulaginStatics::SavePlanePointsNativeToFile(const TCHAR* Path, TArray<FPlaneMissionPointNative> PointsNative)
{
	std::fstream FileStream;

	FileStream.open(Path, std::fstream::out | std::fstream::binary);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: SavePlanePointsNativeToFile: FileStream %s"), FileStream.is_open() ? *FString("OPENED") : *FString("CLOSED"));

	if (!FileStream.is_open())
		return false;

	for (FPlaneMissionPointNative CurrentNativePoint : PointsNative)
	{
		FileStream.write((char*)&CurrentNativePoint.Lat, sizeof(CurrentNativePoint.Lat));
		FileStream.write((char*)&CurrentNativePoint.Lon, sizeof(CurrentNativePoint.Lon));
		FileStream.write((char*)&CurrentNativePoint.Alt, sizeof(CurrentNativePoint.Alt));
		FileStream.write((char*)&CurrentNativePoint.Time, sizeof(CurrentNativePoint.Time));
	}

	FileStream.close();

	return true;
}

TArray<FMissionPointNative> UKulaginStatics::PointsToNativePoints(TArray<FMissionPoint> Points)
{
	TArray<FMissionPointNative> TempPointsNative;
	for (FMissionPoint CurrentPoint : Points)
	{
		TempPointsNative.Add(FMissionPointNative(CurrentPoint));
	}
	return TempPointsNative;
}

TArray<FPlaneMissionPointNative> UKulaginStatics::PlanePointsToNativePoints(TArray<FPlaneMissionPoint> Points)
{
	TArray<FPlaneMissionPointNative> TempPointsNative;
	for (FPlaneMissionPoint CurrentPoint : Points)
	{
		TempPointsNative.Add(FPlaneMissionPointNative(CurrentPoint));
	}
	return TempPointsNative;
}

void UKulaginStatics::AddMissionPointsOffset(UPARAM(ref) TArray<FMissionPoint> &Points, FVector Offset)
{
	for (FMissionPoint &CurrentPoint : Points)
	{
		AddMissionPointOffset(CurrentPoint, Offset);
	}
}

void UKulaginStatics::AddMissionPointOffset(UPARAM(ref) FMissionPoint &Point, FVector Offset)
{
	const FVector TargetLoc = WGS84ToUE(Point, 0.f) + Offset;
	UEToWGS84(Point, TargetLoc, 0.f);
}

double UKulaginStatics::GetValidDoubleFromString(FString Str)
{
	FString FormattedString;
	bool bIsFirstDot = true;

	for (int32 i = 0; i < Str.Len(); i++)
	{
		TCHAR &CurrentChar = Str[i];
		if (!FormattedString.IsEmpty() && bIsFirstDot && (CurrentChar == L',' || CurrentChar == L'.'))
		{ 
			FormattedString.AppendChar(L'.'); 
			bIsFirstDot = false; 
		}
		else if (iswdigit(CurrentChar))
		{
			FormattedString.AppendChar(CurrentChar);
		}
	}

	if (FormattedString.IsEmpty()) return 0.;

	const TCHAR* Native = *FormattedString;
	return std::stod(Native);
}

FString UKulaginStatics::GetValidStringFromDouble(double Val)
{
	if (Val == 0.) return L"0";
	std::wstring String = std::to_wstring(Val);
	while (String.back() == L'0' || String.back() == L'.') String.pop_back();
	const TCHAR* Native = String.c_str();
	return FString(Native);
}

void UKulaginStatics::SetTargetLat(FMissionPoint &Point, FString Lat)
{
	Point.TargetLatLon.Lat = GetValidDoubleFromString(Lat);
}

void UKulaginStatics::SetTargetLon(FMissionPoint &Point, FString Lon)
{
	Point.TargetLatLon.Lon = GetValidDoubleFromString(Lon);
}

void UKulaginStatics::SetPoiLat(FMissionPoint &Point, FString Lat)
{
	Point.poiLat = GetValidDoubleFromString(Lat);
}

void UKulaginStatics::SetPoiLon(FMissionPoint &Point, FString Lon)
{
	Point.poiLon = GetValidDoubleFromString(Lon);
}

FString UKulaginStatics::GetTargetLat(const FMissionPoint &Point)
{
	return GetValidStringFromDouble(Point.TargetLatLon.Lat);
}

FString UKulaginStatics::GetTargetLon(const FMissionPoint &Point)
{
	return GetValidStringFromDouble(Point.TargetLatLon.Lon);
}

FString UKulaginStatics::GetPoiLat(const FMissionPoint &Point)
{
	return GetValidStringFromDouble(Point.poiLat);
}

FString UKulaginStatics::GetPoiLon(const FMissionPoint &Point)
{
	return GetValidStringFromDouble(Point.poiLon);
}

FString UKulaginStatics::GetBinaryFullPath(const FBinaryFilePath &Path)
{
	return Path.GetFullPath();
}

FReferenceLatLon UKulaginStatics::GetReferenceLatLon(const FMissionPoint &Point)
{
	return FReferenceLatLon(Point.TargetLatLon.Lat, Point.TargetLatLon.Lon);
}

bool UKulaginStatics::IsReferenceLatLonValid(const FReferenceLatLon &ReferenceIn)
{
	return ReferenceIn.bIsValid;
}

FVector UKulaginStatics::LatLonToUE(const FLatLon LatLon, float StartAlt)
{
	return WGS84ToUE(LatLon.Lat, Reference.LatReference, LatLon.Lon, Reference.LonReference, 0.f, StartAlt);
}

FVector UKulaginStatics::WGS84ToUE(const FMissionPoint &Point, float StartAlt)
{
	return WGS84ToUE(Point.TargetLatLon.Lat, Reference.LatReference, Point.TargetLatLon.Lon, Reference.LonReference, Point.targetAlt, StartAlt);
}

FVector UKulaginStatics::LogPointToUE(const FLogPoint &Point, float StartAlt)
{
	return WGS84ToUE(Point.Lat, Reference.LatReference, Point.Lon, Reference.LonReference, Point.Alt, StartAlt);
}

FVector UKulaginStatics::WGS84ToUE(double Lat, double LatReference, double Lon, double LonReference, float Alt, float StartAlt)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: WGS84ToUE: v5"));

	std::array<double, 2> Cortesian = wgs84::toCartesian({ LatReference, LonReference }, { Lat, Lon });

	//const double Scale = 10.;
	const float X = float(Cortesian[1] * Scale);
	const float Y = float(Cortesian[0] * Scale);
	const float Z = Alt * float(Scale) + StartAlt;

	const FVector Loc = FVector(X, Y, Z);

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: WGS84ToUE: LatReference = %f, LonReference = %f"), float(LatReference), float(LonReference));
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: WGS84ToUE: Lat = %f, Lon = %f"), float(Lat), float(Lon));
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: WGS84ToUE: X = %f, Y = %f"), Loc.X, Loc.Y);

	return Loc;
}

FLatLon UKulaginStatics::UEToLatLon(FVector Loc)
{
	FLatLon LatLon;
	float Alt;
	UEToWGS84(Loc, LatLon.Lat, Reference.LatReference, LatLon.Lon, Reference.LonReference, Alt, 0.f);
	return LatLon;
}

void UKulaginStatics::UEToWGS84(FMissionPoint &Point, FVector Loc, float StartAlt)
{
	UEToWGS84(Loc, Point.TargetLatLon.Lat, Reference.LatReference, Point.TargetLatLon.Lon, Reference.LonReference, Point.targetAlt, StartAlt);
}

void UKulaginStatics::UEToLogPoint(FLogPoint &Point, FVector Loc, float StartAlt)
{
	UEToWGS84(Loc, Point.Lat, Reference.LatReference, Point.Lon, Reference.LonReference, Point.Alt, StartAlt);
}

void UKulaginStatics::UEToWGS84(FVector Loc, double &Lat, double LatReference, double &Lon, double LonReference, float &Alt, float StartAlt)
{
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: UEToWGS84: v5"));

	//const double Scale = 10.;
	const float X = double(Loc.X) / Scale;
	const float Y = double(Loc.Y) / Scale;
	const float Z = (Loc.Z - StartAlt) / float(Scale);

	std::array<double, 2> WGS84 = wgs84::fromCartesian({ LatReference, LonReference }, { Y, X });

	Lat = WGS84[0];
	Lon = WGS84[1];
	Alt = Z;

	UE_LOG(LogTemp, Warning, TEXT("Kulagin: WGS84ToUE: LatReference = %f, LonReference = %f"), float(LatReference), float(LonReference));
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: WGS84ToUE: Lat = %f, Lon = %f"), float(Lat), float(Lon));
	UE_LOG(LogTemp, Warning, TEXT("Kulagin: WGS84ToUE: X = %f, Y = %f"), Loc.X, Loc.Y);
}

int32 UKulaginStatics::GetZoomMaxIndex()
{
	return FMath::Pow(2, MaxZoom - MinZoom);
}