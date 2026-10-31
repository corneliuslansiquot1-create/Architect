#pragma once

#include "CoreMinimal.h"
#include "ArchitectTypes.generated.h"

/** What kind of surface a cube piece belongs to. Each type gets its own grey-box colour. */
UENUM(BlueprintType)
enum class EArchitectSurface : uint8
{
	Floor,
	BackWall,
	SideWall,
	Ceiling
};

/**
 * Grid conventions used everywhere in ArchitectProto:
 *   i = X = left/right along the room (0 .. Width-1)
 *   j = Y = depth, 0 is the open front facing the camera (0 .. Depth-1)
 *   k = Z = up (0 .. Height-1)
 * A piece's grid coord is the CENTRE of its cube, in cells, relative to the room actor.
 */
namespace ArchitectGrid
{
	/** The six face directions of a cube, in a fixed order (bit 0..5 of a face mask). */
	inline const FIntVector& FaceDir(int32 Index)
	{
		static const FIntVector Dirs[6] =
		{
			FIntVector( 1, 0, 0), FIntVector(-1, 0, 0),
			FIntVector( 0, 1, 0), FIntVector( 0,-1, 0),
			FIntVector( 0, 0, 1), FIntVector( 0, 0,-1)
		};
		return Dirs[Index];
	}

	/** Index of the -Y face (the side that looks towards the camera). */
	constexpr int32 FrontFaceIndex = 3;
}
