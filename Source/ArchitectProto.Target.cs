using UnrealBuildTool;

public class ArchitectProtoTarget : TargetRules
{
	public ArchitectProtoTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ArchitectProto");
	}
}
