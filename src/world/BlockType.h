#ifndef __BLOCKTYPE_H__
#define __BLOCKTYPE_H__

#include "utils/glm.h"
#include "utils/TextureManager.h"
#include "utils/Singleton.h"
#include <vector>
#define TEXCOLS 16

// x, y in [0..1]
typedef glm::vec2 texCoord;

namespace blockTypes
{
	// 1D index for a 2D array
	enum T : uint16_t {
		air = 0,
		grass = 1,
		stone = 2,
		dirt = 3,
		sand = 19,
		sponge = 3*TEXCOLS+1,
		water = 12*TEXCOLS+14,
		water_1 = water+2,
		water_2 = water+3,
		water_3 = water+TEXCOLS+2,
		water_4 = water+TEXCOLS+3,
		maxTypeValue
	};
}
namespace blockPlane {
	enum T {
		top,
		side,
		bottom,
		maxPlanes
	};
}

// bass class for texcoord
class BlockTexCoord {
	protected:
		texCoord *sides; // array or single value
	public:
		virtual inline texCoord getSide(blockPlane::T t) const = 0;
		BlockTexCoord() : sides(NULL) {}
		virtual ~BlockTexCoord() { delete[] sides; }
};

class BlockType;

// class to control animation of blocks
class BlockAnimated : public BlockTexCoord {
	private:
		static const float frameTime; // in seconds
		static BlockType *btype;
		std::vector<blockTypes::T> frames;
		uint32_t current,
				 size;
		float timer;
		void animate(float delta);
		inline blockTypes::T getCurrent() const
		{
			return frames[current];
		}
	public:
		static void setStaticInstance(BlockType *bt); // this need to be called after the creation of OGL context
		BlockAnimated(std::initializer_list<blockTypes::T> frames);
		~BlockAnimated();
		virtual inline texCoord getSide(blockPlane::T p) const; // unused parameter of side because all sides are equal :D
};

class BlockStatic : public BlockTexCoord {
	public:
		BlockStatic(const texCoord& top, const texCoord& bot, const texCoord &side);
		virtual inline texCoord getSide(blockPlane::T p) const
		{
			return sides[p];
		}
};

class BlockType : public Singleton<BlockType> {
	public:
		// get top-left coord
		inline texCoord getTexcoord(blockTypes::T type, blockPlane::T p)
		{
			//TODO : exception pour type==0
			return texCoordMap[type]->getSide(p);
		}

		inline BlockTexCoord& getBlockTexcoord(blockTypes::T type)
		{
			//TODO : exception pour type==0
			return *texCoordMap[type];
		}

		~BlockType();
	private:
		BlockTexCoord* texCoordMap[blockTypes::maxTypeValue];
		friend class Singleton<BlockType>;
		int texWidth, texHeight, texCols;
		BlockType();
};

texCoord BlockAnimated::getSide(blockPlane::T p) const
{
	return btype->getTexcoord(frames[current], p);
}

#endif
