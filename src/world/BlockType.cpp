#include "world/BlockType.h"
#include "utils/dbg.h"

BlockType::BlockType() :
	texWidth(0),
	texHeight(0),
	texCols(16) // XXX must be set manually
{
	TextureManager::getInstance().getTextureSize("data/blocksPack.png", &texWidth, &texHeight);

	// fill texcoord map
	int blockSize = texWidth / texCols,
		texRows = texHeight / blockSize;
	for (int i = 0; i < BlockType::maxTypeValue; i++)
	{
		T t = (T)i;
		auto &side = texCoordMap[t].side;
		switch (t) {
			case T::grass:
				side[BlockType::top] = glm::vec2(0.f, 0.f);
				side[BlockType::side] = glm::vec2(
						(3.f*blockSize)/texWidth,
						(0.f*blockSize)/texHeight
						);
				side[BlockType::bottom] = glm::vec2(
						(4.f*blockSize)/texWidth,
						(0.f*blockSize)/texHeight
						);
				break;
			default:
				side[BlockType::top] = 
					side[BlockType::side] =
					side[BlockType::bottom] = glm::vec2(
							(i % texCols) / (float)texCols,
							(i / texCols) / (float)texRows
							);
				break;
		}
		debug("Added texCoord for type %d: %f, %f", i, texCoordMap[(T)i].side[BlockType::top].x, texCoordMap[(T)i].side[BlockType::top].y);
	}
}

BlockType::~BlockType()
{}

