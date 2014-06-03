#ifndef __BLOCKTYPE_H__
#define __BLOCKTYPE_H__

#include "utils/glm.h"
#include "utils/TextureManager.h"
#include "utils/Singleton.h"

// x, y in [0..1]
typedef glm::vec2 texCoord;

enum blockTypes : uint8_t {
	air = 0,
	grass = 1,
	stone = 2,
	dirt = 3,
	sand = 19,
	maxTypeValue
};

class BlockType : public Singleton<BlockType> {
	public:
		// 1D index for a 2D array
		enum Plane {
			top=0,
			side=1,
			bottom=2,
			maxPlanes
		};

		struct blockTexcoord {
			texCoord side[maxPlanes];
		};

		// get top-left coord
		inline texCoord getTexcoord(blockTypes type, Plane p)
		{
			//TODO : exception pour type==0
			return texCoordMap[type].side[p];
		}

		inline blockTexcoord getBlockTexcoord(blockTypes type)
		{
			//TODO : exception pour type==0
			return texCoordMap[type];
		}

		~BlockType();
	private:
		blockTexcoord texCoordMap[maxTypeValue];
		friend class Singleton<BlockType>;
		int texWidth, texHeight, texCols;
		BlockType();
};

#endif
