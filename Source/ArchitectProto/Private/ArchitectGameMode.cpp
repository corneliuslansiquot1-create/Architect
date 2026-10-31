#include "ArchitectGameMode.h"
#include "ArchitectProto.h"
#include "ArchitectRoom.h"
#include "EngineUtils.h"

AArchitectGameMode::AArchitectGameMode()
{
	RoomClass = AArchitectRoom::StaticClass();
	// Step 1: the default flying spectator pawn (WASD + mouse, E/Q up/down). Replaced by our character in step 2.
}

void AArchitectGameMode::StartPlay()
{
	if (bSpawnRoomIfMissing && RoomClass)
	{
		TActorIterator<AArchitectRoom> It(GetWorld());
		if (!It)
		{
			GetWorld()->SpawnActor<AArchitectRoom>(RoomClass, DefaultRoomLocation, FRotator::ZeroRotator);
			UE_LOG(LogArchitect, Log, TEXT("ArchitectGameMode: no room in the level, spawned one."));
		}
	}

	Super::StartPlay();
}
