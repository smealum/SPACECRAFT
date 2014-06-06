#ifndef __BLOCKTYPE_H__
#define __BLOCKTYPE_H__

#include "utils/glm.h"
#include "utils/TextureManager.h"
#include "utils/Singleton.h"
#include <vector>
#include <list>
#include <cassert>
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
		grass_side = 4,
		flower_red = 13,
		flower_yellow = 14,
		sand = 19,
		tree = 21,
		tree_top = 22,
		grass_blaze = 2*TEXCOLS+8,
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
	enum T : uint8_t {
		top = 0,
		side,
		bottom,
		maxPlanes
	};
}


namespace blockTransparency
{
	enum T : uint8_t {
		// for some pixel...
		opaque = 0, // alpha == 1
		seeThrough = 1, // alpha > 0.01 && < 1
		transparent = 2 // alpha == 0
	};
}

// XXX XXX
// J'ai eut un peu de mal à m'intégré au code.
// Pourquoi c'est si compliqué ?
// Si quelqu'un veut bien m'intégré.
extern int blockTileID[blockTypes::maxTypeValue][blockPlane::maxPlanes];
extern uint8_t blockTrans[blockTypes::maxTypeValue];

inline int getBlockID(int blockType, int plane)
{
	return blockTileID[blockType][plane]-1;
}

void blockTypeLoadValues();
// fin ajout Arthur

class BlockType;

// bass class for texcoord
class BlockTexCoord {
	protected:
		blockTransparency::T trans;
		static BlockType *btype;
	public:
		virtual texCoord getSide(blockPlane::T t) const = 0;
		inline blockTransparency::T getTransparency() const { return trans; }
		inline void setTransparency(blockTransparency::T type) { trans = type; }
		BlockTexCoord() {}
		virtual ~BlockTexCoord() {}
		static void setStaticInstance(BlockType *bt); // this need to be called after the creation of OGL context
};

// class to control animation of blocks
class BlockAnimated : public BlockTexCoord {
	private:
		static const float frameTime; // in seconds
		static std::list<BlockAnimated*> list;
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
		static void animation(float delta);
		BlockAnimated(std::initializer_list<blockTypes::T> frames);
		~BlockAnimated();
		virtual texCoord getSide(blockPlane::T p) const; // unused parameter of side because all sides are equal :D
};

class BlockStatic : public BlockTexCoord {
	private:
		std::vector<blockTypes::T> sides;
	public:
		BlockStatic(std::initializer_list<blockTypes::T> sdes);
		virtual texCoord getSide(blockPlane::T p) const;
};

class BlockType : public Singleton<BlockType> {
	public:
		// get top-left coord
		inline texCoord getTexcoord(blockTypes::T type) // this refers to a plane
		{
			return texCoords[type];
		}

		inline texCoord getTexcoord(blockTypes::T type, blockPlane::T p) // this refers to a plane
		{
			return blocks[type]->getSide(p);
		}

		inline BlockTexCoord& getBlockTexcoord(blockTypes::T type) // this refers to the block type
		{
			return *blocks[type];
		}


		~BlockType();
	private:
		BlockTexCoord* blocks[blockTypes::maxTypeValue];
		texCoord texCoords[blockTypes::maxTypeValue];
		friend class Singleton<BlockType>;
		int texWidth, texHeight, texCols;
		BlockType();
};

#endif
