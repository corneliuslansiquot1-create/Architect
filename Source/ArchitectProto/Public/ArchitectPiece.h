#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArchitectTypes.h"
#include "ArchitectPiece.generated.h"

class UStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;

/**
 * One 1 m cube of the room.
 *
 * How it looks seamless: the cube BODY uses the dark "hidden face" material.
 * Every face that can be seen in normal play gets a thin coloured PANEL laid on it.
 * Neighbouring panels line up edge to edge, so the room reads as one smooth surface.
 * When the seams open (later step) the pieces shrink a little and the dark sides show.
 *
 * You don't place these by hand: ArchitectRoom spawns them when you press Play.
 */
UCLASS()
class ARCHITECTPROTO_API AArchitectPiece : public AActor
{
	GENERATED_BODY()

public:
	AArchitectPiece();

	/** Called by the room right after spawning. FaceMask bit N = put a panel on FaceDir(N). */
	void InitPiece(const FIntVector& InGridCoord, EArchitectSurface InSurface, uint8 FaceMask, float CellSize,
		UStaticMesh* CubeMesh, UStaticMesh* PanelMesh, UMaterialInterface* SurfaceMaterial, UMaterialInterface* HiddenMaterial);

	/** Grid position (i, j, k) this piece was built at. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Architect")
	FIntVector GridCoord = FIntVector::ZeroValue;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Architect")
	EArchitectSurface Surface = EArchitectSurface::Floor;

	/** Root that everything hangs off. Scaling this opens/closes the seams. */
	UPROPERTY(VisibleAnywhere, Category = "Architect")
	TObjectPtr<USceneComponent> PieceRoot;

	/** The cube itself: dark material, and it carries the collision. */
	UPROPERTY(VisibleAnywhere, Category = "Architect")
	TObjectPtr<UStaticMeshComponent> Body;

	/** The coloured panels on the visible faces. */
	UPROPERTY(VisibleInstanceOnly, Category = "Architect")
	TArray<TObjectPtr<UStaticMeshComponent>> FacePanels;

	/** How far (cm) each panel sits in front of the cube face, so they don't flicker against each other. */
	static constexpr float PanelOffset = 0.2f;
};
