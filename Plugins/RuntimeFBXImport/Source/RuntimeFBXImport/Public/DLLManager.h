// Copyright 2018-2019, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "PlatformFeatures.h"
#include "Windows/WindowsPlatformProcess.h"


static class DLLManager
{
private:


	static bool ImportMethodOpenFileDialogue();

public:

	/**
	* Loads DLL from the given name.
	* @param  DLLName  The DLL to load.
	* @returns true if the DLL is successfully loaded.
	*/
	static bool LoadDLL(FString DLLName);

	/**
	* UnLoads the currently loaded DLL.
	*/
	static void UnLoadDLL();

	/**
	* Opens a Dialogue window to select files of given type.
	* @param  FileTypes  The filetypes of the files to be selected.
	* @returns FileNames  The Selected Files.
	*/
	static bool GetOpenFileNames(char* FileTypes , TArray<FString> &FileNames);


};

