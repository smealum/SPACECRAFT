#ifndef __BLOCKTYPE_H__
#define __BLOCKTYPE_H__

#include "utils/glm.h"
#include "utils/TextureManager.h"
#include "utils/Singleton.h"
#include <vector>
#include <list>
#include <cassert>

#define TEXCOLS 16

namespace blockTypes
{
	// 1D index for a 2D array
	enum T : uint16_t {
		air = 0,
		grass = 1,
		stone = 2,
		dirt = 3,
		grass_side = 4,
		flower_red = 13,
		flower_yellow = 14,
		sand = 19,
		tree = 21,
		tree_top = 22,
		grass_blaze = 2*TEXCOLS+8,
		sponge = 3*TEXCOLS+1,
		water = 12*TEXCOLS+14,
		water_1 = water+1,
		water_2 = water+2,
		water_3 = water+TEXCOLS+1,
		water_4 = water+TEXCOLS+2,
		max
	};
}

namespace blockPlane {
	enum T : uint8_t {
		top = 0,
		side,
		bottom,
		max
	};
}


namespace blockTransparency
{
	enum T : uint8_t {
		// for some pixel...
		opaque = 0, // alpha == 1
		seeThrough = 1, // alpha > 0.01 && < 1
		transparent = 2, // alpha == 0
		invisible = 3, // nothing to draw
		max
	};
}

namespace blockStyle
{
	enum T : uint8_t {
		normal = 0, // => box
		sprite = 1, // => flower style
		max
	};
}

// donne l'ID de tile pour un bloc et une orientation
extern int blockTileID[blockTypes::max][blockPlane::max];

// donne l'ID de transparence d'un bloc
extern uint8_t blockTransparencyID[blockTypes::max];

// donne l'ID de style d'un bloc
extern uint8_t blockStyleID[blockTypes::max];

bool blockShouldBeFace(int type1, int type2);

inline int getBlockID(int blockType, int plane)
{
	return blockTileID[blockType][plane]-1;
}
void blockTypeLoadValues();

#endif
