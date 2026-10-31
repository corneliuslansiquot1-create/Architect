#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArchitectTypes.h"
#include "ArchitectRoom.generated.h"

class AArchitectPiece;
class UStaticMesh;
class UMaterialInterface;
class UMaterialInstanceDynamic;

/**
 * The room. When play starts it spawns one ArchitectPiece per solid grid cell:
 * floor, back wall, ceiling and two side walls, open towards the camera (-Y).
 *
 * Everything you'd want to tweak is in the Details panel under "Architect".
 * The game mode spawns one of these automatically if the level doesn't have one.
 */
UCLASS()
class ARCHITECTPROTO_API AArchitectRoom : public AActor
{
	GENERATED_BODY()

public:
	AArchitectRoom();

	// ----- Layout -----

	/** Cells along X (left to right). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Architect|Layout", meta = (ClampMin = "3"))
	int32 Width = 18;

	/** Cells along Z (floor to ceiling, including both). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Architect|Layout", meta = (ClampMin = "3"))
	int32 Height = 5;

	/** Cells along Y (front edge to back wall, including the back wall). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Architect|Layout", meta = (ClampMin = "2"))
	int32 Depth = 5;

	/** Size of one cube in cm. Keep at 100 (1 m) for the prototype. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Architect|Layout", meta = (ClampMin = "10"))
	float CellSize = 100.f;

	/** Cells to leave empty (e.g. a window hole). Grid coords (i, j, k). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Architect|Layout")
	TArray<FIntVector> RemovedCells;

	/** Put a placeholder lamp under the middle of the ceiling. It is attached to that ceiling piece. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Architect|Layout")
	bool bAddCeilingLamp = true;

	// ----- Look -----

	UPROPERTY(EditAnywhere, Category = "Architect|Look")
	TObjectPtr<UStaticMesh> CubeMesh;

	UPROPERTY(EditAnywhere, Category = "Architect|Look")
	TObjectPtr<UStaticMesh> PanelMesh;

	/** Base material; we make a coloured copy per surface type. Needs a vector parameter named ColorParameterName. */
	UPROPERTY(EditAnywhere, Category = "Architect|Look")
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY(EditAnywhere, Category = "Architect|Look")
	FName ColorParameterName = TEXT("Color");

	/** Grey-box colour for each surface type. */
	UPROPERTY(EditAnywhere, Category = "Architect|Look")
	TMap<EArchitectSurface, FLinearColor> SurfaceColors;

	/** Colour of the pieces' hidden faces (the "construction material" seen in the seams). */
	UPROPERTY(EditAnywhere, Category = "Architect|Look")
	FLinearColor HiddenFaceColor = FLinearColor(0.015f, 0.015f, 0.018f);

	// ----- Runtime -----

	/** Spawn all pieces (removes old ones first). Runs automatically at BeginPlay. */
	UFUNCTION(BlueprintCallable, Category = "Architect")
	void BuildRoom();

	UFUNCTION(BlueprintCallable, Category = "Architect")
	void ClearRoom();

	/** The piece at a grid coord, or null. */
	UFUNCTION(BlueprintCallable, Category = "Architect")
	AArchitectPiece* GetPieceAt(FIntVector Coord) const;

	/** Room-local position (cm) of a grid cell's centre. */
	FVector GridToLocal(const FIntVector& Coord) const { return FVector(Coord) * CellSize; }

	/** Is there a piece in this cell in the starting layout? */
	bool IsSolidCell(const FIntVector& Coord) const;

	/** Which surface a solid cell belongs to (only valid if IsSolidCell). */
	EArchitectSurface ClassifyCell(const FIntVector& Coord) const;

	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Architect|Runtime")
	TArray<TObjectPtr<AArchitectPiece>> Pieces;

protected:
	virtual void BeginPlay() override;

private:
	bool IsInBounds(const FIntVector& C) const
	{
		return C.X >= 0 && C.X < Width && C.Y >= 0 && C.Y < Depth && C.Z >= 0 && C.Z < Height;
	}

	/** Which faces of this cell get a coloured panel (bit N = FaceDir(N)). */
	uint8 ComputeFaceMask(const FIntVector& Coord) const;

	/** Fast lookup from grid coord to piece. (Pieces array above keeps them alive.) */
	TMap<FIntVector, AArchitectPiece*> PieceGrid;

	UPROPERTY(Transient)
	TMap<EArchitectSurface, TObjectPtr<UMaterialInstanceDynamic>> SurfaceMaterials;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> HiddenMaterial;
};
