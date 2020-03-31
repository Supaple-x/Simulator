// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_FuncLib.h"
#include "Engine.h"
//#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
//#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
//#include "vehicles/multirotor/api/MultirotorRpcLibClient.hpp"

void UCPP_FuncLib::ExecuteDroneShell()
{
	static FString FilePath = FPaths::ProjectUserDir() + "DroneShell/DroneShell.exe";
	static FString Attributes = "";
	FPlatformProcess::CreateProc(*FilePath, *Attributes, false, true, false, NULL, 0, NULL, NULL);
}

/*void UCPP_FuncLib::Test()
{
	msr::airlib::MultirotorRpcLibClient client;
	client.enableApiControl(true);
	client.armDisarm(true);
	client.takeoffAsync(5)->waitOnLastTask();
}*/

bool UCPP_FuncLib::CreateDroneTracesDirectory()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	const FString path = FPaths::ProjectUserDir() + "DroneTraces";

	if (!PlatformFile.DirectoryExists(*path))
	{
		PlatformFile.CreateDirectory(*path);

		if (!PlatformFile.DirectoryExists(*path))
			return false;
	}

	return true;
}

bool UCPP_FuncLib::UpdateDroneTracesData(int32 DroneNumber, TArray<FString> DataToAdd)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	const FString path = FPaths::ProjectUserDir() + "DroneTraces/";
	FString ResultingString = "";
	FString FileFullPath = path + "/Drone" + FString::FromInt(DroneNumber) + ".txt";

	if (PlatformFile.DirectoryExists(*path))
	{
		for (int32 i = 0; i < DataToAdd.Num(); i++)
			ResultingString += FString::FromInt(i+1) + DataToAdd[i] + "\n";

		return FFileHelper::SaveStringToFile(ResultingString, *FileFullPath);
	}

	return false;
}
/*
void UCPP_FuncLib::OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames)
{
	if (GEngine)
	{
		if (GEngine->GameViewport)
		{
			//void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			if (DesktopPlatform)
			{
				//Opening the file picker!
				uint32 SelectionFlag = 0; //A value of 0 represents single file selection while a value of 1 represents multiple file selection
				DesktopPlatform->OpenFileDialog(GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle(), DialogTitle, DefaultPath, FString(""), FileTypes, SelectionFlag, OutFileNames);
			}
		}
	}
}

void UCPP_FuncLib::PrintData(const FString& File)
{
	//Parse the data into a string array
	TArray<FString> LoadedText;
	FFileHelper::LoadFileToStringArray(LoadedText, *File);
	//Print the contents
	for (int32 i = 0; i < LoadedText.Num(); i++)
	{
		GLog->Log(LoadedText[i]);
	}
}*/