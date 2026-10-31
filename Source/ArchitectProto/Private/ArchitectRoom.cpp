#include "ArchitectRoom.h"
#include "ArchitectProto.h"
#include "ArchitectPiece.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AArchitectRoom::AArchitectRoom()
{
	PrimaryActorTick.bCanEverTick = false; // turned on in a later step for the rebuild player

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Movable);
	RootComponent = Root;

	// Engine starter content that ships with every Unreal install (no download needed).
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	CubeMesh = CubeFinder.Object;
	PanelMesh = PlaneFinder.Object;
	BaseMaterial = MatFinder.Object;

	SurfaceColors.Add(EArchitectSurface::Floor,    FLinearColor(0.20f, 0.20f, 0.21f));
	SurfaceColors.Add(EArchitectSurface::BackWall, FLinearColor(0.45f, 0.45f, 0.44f));
	SurfaceColors.Add(EArchitectSurface::SideWall, FLinearColor(0.36f, 0.36f, 0.36f));
	SurfaceColors.Add(EArchitectSurface::Ceiling,  FLinearColor(0.28f, 0.28f, 0.29f));
}

void AArchitectRoom::BeginPlay()
{
	Super::BeginPlay();
	BuildRoom();
}

bool AArchitectRoom::IsSolidCell(const FIntVector& C) const
{
	if (!IsInBounds(C) || RemovedCells.Contains(C))
	{
		return false;
	}
	const bool bFloor    = C.Z == 0;
	const bool bCeiling  = C.Z == Height - 1;
	const bool bBackWall = C.Y == Depth - 1;
	const bool bSideWall = C.X == 0 || C.X == Width - 1;
	return bFloor || bCeiling || bBackWall || bSideWall;
}

EArchitectSurface AArchitectRoom::ClassifyCell(const FIntVector& C) const
{
	// Order matters for edges/corners: floor and ceiling win, then back wall, then side walls.
	if (C.Z == 0)          return EArchitectSurface::Floor;
	if (C.Z == Height - 1) return EArchitectSurface::Ceiling;
	if (C.Y == Depth - 1)  return EArchitectSurface::BackWall;
	return EArchitectSurface::SideWall;
}

uint8 AArchitectRoom::ComputeFaceMask(const FIntVector& C) const
{
	// A face gets a coloured panel when it is not pressed against another piece
	// AND it can be seen: it either looks into the room, or it is the front edge facing the camera.
	// Faces pressed against neighbours stay dark: those are the "hidden faces".
	uint8 Mask = 0;
	for (int32 F = 0; F < 6; ++F)
	{
		const FIntVector N = C + ArchitectGrid::FaceDir(F);
		const bool bNeighbourSolid = IsSolidCell(N);
		const bool bVisible = IsInBounds(N) || F == ArchitectGrid::FrontFaceIndex;
		if (!bNeighbourSolid && bVisible)
		{
			Mask |= (1 << F);
		}
	}
	return Mask;
}

void AArchitectRoom::ClearRoom()
{
	for (AArchitectPiece* Piece : Pieces)
	{
		if (IsValid(Piece))
		{
			Piece->Destroy();
		}
	}
	Pieces.Reset();
	PieceGrid.Reset();
}

AArchitectPiece* AArchitectRoom::GetPieceAt(FIntVector Coord) const
{
	AArchitectPiece* const* Found = PieceGrid.Find(Coord);
	return Found ? *Found : nullptr;
}

void AArchitectRoom::BuildRoom()
{
	ClearRoom();

	UWorld* World = GetWorld();
	if (!World || !CubeMesh || !PanelMesh || !BaseMaterial)
	{
		UE_LOG(LogArchitect, Error, TEXT("ArchitectRoom: missing world, mesh or material - room not built."));
		return;
	}

	// One coloured material per surface type, plus the dark hidden-face material.
	SurfaceMaterials.Reset();
	for (const TPair<EArchitectSurface, FLinearColor>& Pair : SurfaceColors)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		MID->SetVectorParameterValue(ColorParameterName, Pair.Value);
		SurfaceMaterials.Add(Pair.Key, MID);
	}
	HiddenMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	HiddenMaterial->SetVectorParameterValue(ColorParameterName, HiddenFaceColor);

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	int32 PanelCount = 0;
	for (int32 K = 0; K < Height; ++K)
	for (int32 J = 0; J < Depth; ++J)
	for (int32 I = 0; I < Width; ++I)
	{
		const FIntVector C(I, J, K);
		if (!IsSolidCell(C))
		{
			continue;
		}

		const EArchitectSurface Surface = ClassifyCell(C);
		const FTransform WorldXform = FTransform(GridToLocal(C)) * GetActorTransform();

		AArchitectPiece* Piece = World->SpawnActor<AArchitectPiece>(AArchitectPiece::StaticClass(), WorldXform, Params);
		if (!Piece)
		{
			continue;
		}

		UMaterialInterface* SurfaceMat = SurfaceMaterials.Contains(Surface) ? SurfaceMaterials[Surface].Get() : BaseMaterial.Get();
		Piece->InitPiece(C, Surface, ComputeFaceMask(C), CellSize, CubeMesh, PanelMesh, SurfaceMat, HiddenMaterial);
		Piece->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
#if WITH_EDITOR
		Piece->SetActorLabel(FString::Printf(TEXT("Piece_%d_%d_%d"), I, J, K));
#endif
		PanelCount += Piece->FacePanels.Num();
		Pieces.Add(Piece);
		PieceGrid.Add(C, Piece);
	}

	// Placeholder ceiling lamp, attached to the ceiling piece so it will move with it.
	if (bAddCeilingLamp)
	{
		if (AArchitectPiece* LampPiece = GetPieceAt(FIntVector(Width / 2, (Depth - 1) / 2, Height - 1)))
		{
			UPointLightComponent* Lamp = NewObject<UPointLightComponent>(LampPiece, TEXT("CeilingLamp"));
			Lamp->SetMobility(EComponentMobility::Movable);
			Lamp->SetupAttachment(LampPiece->PieceRoot);
			Lamp->SetRelativeLocation(FVector(0.f, 0.f, -CellSize * 0.5f - 40.f));
			Lamp->SetIntensityUnits(ELightUnits::Candelas);
			Lamp->SetIntensity(60.f);
			Lamp->SetAttenuationRadius(1500.f);
			Lamp->SetLightColor(FLinearColor(1.f, 0.85f, 0.65f));
			Lamp->RegisterComponent();
			LampPiece->AddInstanceComponent(Lamp);
		}
	}

	UE_LOG(LogArchitect, Log, TEXT("ArchitectRoom: built %d pieces (%d visible face panels) for a %d x %d x %d room."),
		Pieces.Num(), PanelCount, Width, Height, Depth);
}
