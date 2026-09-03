// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Game : ModuleRules
{
	public Game(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore"  });

		PrivateDependencyModuleNames.AddRange(new string[] { "DeveloperSettings", "MoviePlayer", "GMP", "AdvancedMVVM", "ModelViewViewModel" });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "RenderCore", "RHI", "ImageCore" });
        }
    }
}
