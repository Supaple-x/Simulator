// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CPP_FuncLib.generated.h"

/**
 * 
 */
UCLASS()
class KULAGIN_API UCPP_FuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Executer")
	static void ExecuteDroneShell();

	UFUNCTION(BlueprintCallable, Category = "DroneTraces")
	static bool CreateDroneTracesDirectory();
	
	UFUNCTION(BlueprintCallable, Category = "DroneTraces")
	static bool UpdateDroneTracesData(int32 DroneNumber, TArray<FString> DataToAdd);

	/*UFUNCTION(BlueprintCallable, Category = "Test")
		void Test();

	UFUNCTION(BlueprintCallable, Category = "FilePicker")
	void OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& FileTypes, TArray<FString>& OutFileNames);

	UFUNCTION(BlueprintCallable, Category = "FilePicker")
	void PrintData(const FString& File);*/
};
