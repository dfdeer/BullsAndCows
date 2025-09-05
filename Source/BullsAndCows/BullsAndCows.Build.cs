using UnrealBuildTool;

public class BullsAndCows : ModuleRules
{
	public BullsAndCows(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput", 

			// UI
			"UMG",
            "Slate",
            "SlateCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.AddRange(new string[] { "BullsAndCows" });
    }
}
