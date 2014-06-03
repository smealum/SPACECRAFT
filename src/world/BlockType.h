#ifndef __BLOCKTYPE_H__
#define __BLOCKTYPE_H__

#include "utils/glm.h"
#include "utils/TextureManager.h"
#include "utils/Singleton.h"
#include <map>

// x, y in [0..1]
typedef glm::vec2 texCoord;

class BlockType : public Singleton<BlockType> {
	enum Plane {
		top,
		side,
		bottom,
		maxPlanes
	};
	struct blockTexcoord {
		texCoord side[maxPlanes];
	};
	public:
		// 1D index for a 2D array
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
	private:
		std::map<T, blockTexcoord> texCoordMap;
		friend class Singleton<BlockType>;
		int texWidth, texHeight, texCols;
		BlockType();
		~BlockType();
};

#endif
