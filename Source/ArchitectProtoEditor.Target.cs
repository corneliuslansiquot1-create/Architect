using UnrealBuildTool;

public class ArchitectProtoEditorTarget : TargetRules
{
	public ArchitectProtoEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ArchitectProto");
	}
}
