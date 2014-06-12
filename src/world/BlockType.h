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
		tree_foliage = tree + TEXCOLS * 2,
		grass_blaze = 2*TEXCOLS+8,
		sponge = 3*TEXCOLS+1,
		water = 12*TEXCOLS+14,
		water_1 = water+1,
		water_2 = water+2,
		water_3 = water+TEXCOLS+1,
		water_4 = water+TEXCOLS+2,
		snow = 3 + 4*TEXCOLS,
		max = 16*16
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

// XXX XXX
// J'ai eut un peu de mal à m'intégré au code.
// Pourquoi c'est si compliqué ?
// Si quelqu'un veut bien m'intégré.
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
// On peut supprimer le reste ?????
// fin ajout Arthur
//////////////////////////////////////////////////////////

class BlockType;

// bass class for texcoord
class BlockTexCoord {
	protected:
		blockTransparency::T trans;
		blockStyle::T style;
		static BlockType *btype;
	public:
		virtual texCoord getSide(blockPlane::T t) const = 0;
		inline blockTransparency::T getTransparency() const { return trans; }
		inline blockStyle::T getStyle() const { return style; }
		inline void setTransparency(blockTransparency::T type) { trans = type; }
		inline void setStyle(blockStyle::T type) { style = type; }
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
		uint32_t current;
		float timer;
		void animate(float delta);
	public:
		inline blockTypes::T getCurrent() const
		{
			return frames[current];
		}
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

		bool shouldBeFace(blockTypes::T type1, blockTypes::T type2);

		~BlockType();
	private:
		BlockTexCoord* blocks[blockTypes::max];
		texCoord texCoords[blockTypes::max];
		friend class Singleton<BlockType>;
		int texWidth, texHeight, texCols;
		BlockType();
};

#endif
