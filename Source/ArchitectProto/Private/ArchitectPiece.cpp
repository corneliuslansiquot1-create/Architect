#include "ArchitectPiece.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

AArchitectPiece::AArchitectPiece()
{
	// Pieces don't need to tick on their own; the room moves them.
	PrimaryActorTick.bCanEverTick = false;

	PieceRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PieceRoot"));
	PieceRoot->SetMobility(EComponentMobility::Movable);
	RootComponent = PieceRoot;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(PieceRoot);
	Body->SetMobility(EComponentMobility::Movable);
	Body->SetCollisionProfileName(TEXT("BlockAll"));
}

void AArchitectPiece::InitPiece(const FIntVector& InGridCoord, EArchitectSurface InSurface, uint8 FaceMask, float CellSize,
	UStaticMesh* CubeMesh, UStaticMesh* PanelMesh, UMaterialInterface* SurfaceMaterial, UMaterialInterface* HiddenMaterial)
{
	GridCoord = InGridCoord;
	Surface = InSurface;

	// The engine's basic cube and plane are both 100 cm, centred on their pivot.
	const float Scale = CellSize / 100.f;

	Body->SetStaticMesh(CubeMesh);
	Body->SetRelativeScale3D(FVector(Scale));
	Body->SetMaterial(0, HiddenMaterial);

	for (int32 FaceIndex = 0; FaceIndex < 6; ++FaceIndex)
	{
		if ((FaceMask & (1 << FaceIndex)) == 0)
		{
			continue;
		}

		const FVector Dir = FVector(ArchitectGrid::FaceDir(FaceIndex));

		UStaticMeshComponent* Panel = NewObject<UStaticMeshComponent>(this, *FString::Printf(TEXT("FacePanel_%d"), FaceIndex));
		Panel->SetMobility(EComponentMobility::Movable);
		Panel->SetupAttachment(PieceRoot);
		Panel->SetStaticMesh(PanelMesh);
		Panel->SetMaterial(0, SurfaceMaterial);
		Panel->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// The plane mesh faces +Z. Turn it so it faces outwards along Dir, and push it onto that face.
		Panel->SetRelativeLocation(Dir * (CellSize * 0.5f + PanelOffset));
		Panel->SetRelativeRotation(FRotationMatrix::MakeFromZ(Dir).Rotator());
		Panel->SetRelativeScale3D(FVector(Scale));

		Panel->RegisterComponent();
		AddInstanceComponent(Panel); // makes it show in the Details panel while playing
		FacePanels.Add(Panel);
	}
}
