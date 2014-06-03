#ifndef __BLOCKTYPE_H__
#define __BLOCKTYPE_H__

#include "utils/glm.h"
#include "utils/TextureManager.h"
#include "utils/Singleton.h"

// x, y in [0..1]
typedef glm::vec2 texCoord;

class BlockType : public Singleton<BlockType> {
	public:
		// 1D index for a 2D array
		enum Plane {
			top,
			side,
			bottom,
			maxPlanes
		};
		struct blockTexcoord {
			texCoord side[maxPlanes];
		};
		enum T {
			grass = 0,
			stone = 1,
			dirt = 2,
			sand = 18,
			maxTypeValue
		};
		// get top-left coord
		inline texCoord getTexcoord(T type, Plane p)
		{
			return texCoordMap[type].side[p];
		}
		inline blockTexcoord getBlockTexcoord(T type)
		{
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
