// Copyright 2018-2019, Athian Games. All Rights Reserved. 

#include "DLLManager.h"
#include "Interfaces/IPluginManager.h"

static void *v_FBXdllHandle;

typedef char*(*_OpenFBXFileDialogue)(char* DialogueTitle, char* FileTypes);

static _OpenFBXFileDialogue m_OpenFBXFileDialogueFromDll;


bool DLLManager::LoadDLL(FString DLLName)
{
	FString Pluginpath = IPluginManager::Get().FindPlugin(TEXT("RuntimeFBXImport"))->GetBaseDir();
	FString DllPath = FPaths::Combine(*Pluginpath, TEXT("Source/Libraries"), DLLName);

	if (FPaths::FileExists(DllPath))
	{
		v_FBXdllHandle = FPlatformProcess::GetDllHandle(*DllPath);
		if (v_FBXdllHandle != NULL)
		{
			return true;
		}
	}
	return false;
}

void DLLManager::UnLoadDLL()
{
	if (v_FBXdllHandle != NULL)
	{
		m_OpenFBXFileDialogueFromDll = NULL;
		FPlatformProcess::FreeDllHandle(v_FBXdllHandle);
		v_FBXdllHandle = NULL;
	}
}

bool DLLManager::ImportMethodOpenFileDialogue()
{
	if (v_FBXdllHandle != NULL)
	{
		m_OpenFBXFileDialogueFromDll = NULL;
		FString procName = "OpenFileDialogue";
		m_OpenFBXFileDialogueFromDll = (_OpenFBXFileDialogue)FPlatformProcess::GetDllExport(v_FBXdllHandle, *procName);
		if (m_OpenFBXFileDialogueFromDll != NULL)
		{
			return true;
		}
	}
	return false;
}

bool DLLManager::GetOpenFileNames(char* FileTypes, TArray<FString> &FileNames)
{
	if (ImportMethodOpenFileDialogue())
	{
		char* DialogueTitle = "Select the file to import from";

		char* returnChar = m_OpenFBXFileDialogueFromDll(DialogueTitle, FileTypes);
		FString RString = ANSI_TO_TCHAR(returnChar);
		if (!RString.IsEmpty())
		{
			FString LString;
			while (!RString.IsEmpty())
			{
				FString NString = RString;
				RString.Empty();
				if (NString.Contains(","))
				{
					NString.Split(",", &LString, &RString, ESearchCase::CaseSensitive);
				}
				else
				{
					LString = NString;
				}
				if (FPaths::FileExists(LString))
				{
					FileNames.Add(LString);
				}
			}

			return true;

		}
	}

	return false;
}
