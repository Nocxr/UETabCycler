using UnrealBuildTool;

public class TabCycler : ModuleRules
{
    public TabCycler(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Slate",
            "SlateCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "UnrealEd",
            "LevelEditor",
            "DeveloperSettings"
        });
    }
}