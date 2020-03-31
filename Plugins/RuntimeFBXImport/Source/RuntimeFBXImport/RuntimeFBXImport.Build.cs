// Copyright 2018-2019, Athian Games. All Rights Reserved. 

using System.IO;
using System;
using UnrealBuildTool;

public class RuntimeFBXImport : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Source/ThirdParty/")); }
    }

    private string CustomDLLPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Libraries/")); }
    }


    public RuntimeFBXImport(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "ProceduralMeshComponent",
                "RenderCore",
                "RHI",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "AppFramework",
                "UMG",
                "InputCore",
                "ApplicationCore",
                "ImageWrapper"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "AppFramework", "Projects"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
        // Libraries Path      
        string FBXLibrariesPath = Path.Combine(ThirdPartyPath, "fbx", "Libraries");

        PublicLibraryPaths.Add(FBXLibrariesPath);

        PublicAdditionalLibraries.Add(Path.Combine(FBXLibrariesPath, "libfbxsdk-md.lib"));
        // Include path
        string FBXIncludePath = Path.Combine(ThirdPartyPath, "fbx", "Includes");

        PublicIncludePaths.Add(FBXIncludePath);

        string BaseDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
        string LibrariesDirectory = Path.Combine(BaseDirectory, "Source/Libraries");

        RuntimeDependencies.Add(Path.Combine(LibrariesDirectory, "RuntimeFBXImportLibrary.dll"));

        // Definitions
        PublicDefinitions.Add(string.Format("WITH_FBX_BINDING=1"));


    }
}
