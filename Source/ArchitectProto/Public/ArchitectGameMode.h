#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArchitectGameMode.generated.h"

class AArchitectRoom;

/**
 * Game mode for the prototype. Set as the project default in Config/DefaultEngine.ini.
 * If the level has no ArchitectRoom, it spawns one so you only need an empty-ish level.
 */
UCLASS()
class ARCHITECTPROTO_API AArchitectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArchitectGameMode();

	virtual void StartPlay() override;

	UPROPERTY(EditAnywhere, Category = "Architect")
	bool bSpawnRoomIfMissing = true;

	UPROPERTY(EditAnywhere, Category = "Architect")
	TSubclassOf<AArchitectRoom> RoomClass;

	/** Where the auto-spawned room goes. Default centres it on X and puts the floor's top at Z = 0. */
	UPROPERTY(EditAnywhere, Category = "Architect")
	FVector DefaultRoomLocation = FVector(-850.f, 0.f, -50.f);
};
