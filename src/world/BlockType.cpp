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

	for (int i = 1; i < blockTypes::maxTypeValue; i++)
	{
		blockTypes t = (blockTypes)i;
		auto &side = texCoordMap[t].side;

		switch (t) {
			case grass:
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
							((i-1) % texCols) / (float)texCols,
							((i-1) / texCols) / (float)texRows
							);
				break;
		}

		// debug("Added texCoord for type %d: %f, %f", i, texCoordMap[(blockTypes)i].side[BlockType::top].x, texCoordMap[(blockTypes)i].side[BlockType::top].y);
	}
}

BlockType::~BlockType()
{}
