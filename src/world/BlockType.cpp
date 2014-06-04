#include "world/BlockType.h"
#include "utils/dbg.h"

const float BlockAnimated::frameTime(0.5f);
BlockType *BlockAnimated::btype(NULL);

BlockType::BlockType() :
	texWidth(0),
	texHeight(0),
	texCols(TEXCOLS) // XXX must be set manually
{
	TextureManager::getInstance().getTextureSize("data/blocksPack.png", &texWidth, &texHeight);

	// fill texcoord map
	int blockSize = texWidth / texCols,
		texRows = texHeight / blockSize;

	debug("%u texcoord will be generated", blockTypes::maxTypeValue);
	for (uint32_t i = 1; i < blockTypes::maxTypeValue; i++)
	{
		blockTypes::T t = (blockTypes::T)i;
		auto &side = texCoordMap[t];

		switch (t) {
			case blockTypes::grass:
				side = new BlockStatic(
						glm::vec2(0.f, 0.f),
						glm::vec2(
							(2.f*blockSize)/texWidth,
							(0.f*blockSize)/texHeight
							),
						glm::vec2(
							(3.f*blockSize)/texWidth,
							(0.f*blockSize)/texHeight
							)
						);
				break;
			//case blockTypes::water:
				//side = new BlockAnimated({
						//blockTypes::water,
						//blockTypes::water_1,
						//blockTypes::water_2,
						//blockTypes::water_3,
						//blockTypes::water_4
						//});
				//break;
			default:
				auto v = glm::vec2(
							((i-1) % texCols) / (float)texCols,
							((i-1) / texCols) / (float)texRows
							);

				side = new BlockStatic(v, v, v);
				break;
		}

		debug("Added texCoord for type %d: %s", i, glm::to_string(texCoordMap[i]->getSide(blockPlane::top)).c_str());
	}
	auto p = texCoordMap[blockTypes::water];
	debug("water(%u): %p, framenow: %s", blockTypes::water, p, glm::to_string(p->getSide(blockPlane::top)).c_str());
	BlockAnimated::setStaticInstance(this);
}

BlockType::~BlockType()
{
	// free the blocktypes
	for (uint32_t i = 1; i < blockTypes::maxTypeValue; ++i)
		delete texCoordMap[i];
}

BlockAnimated::BlockAnimated(std::initializer_list<blockTypes::T> frames) :
	BlockTexCoord(),
	frames(frames),
	current(0),
	timer(0.f)
{
	// sides remain unused
}

void BlockAnimated::animate(float delta)
{
	timer += delta;
	while (timer > frameTime)
		timer -= frameTime, current++;
	if (current >= size)
		current -= size;
}

BlockAnimated::~BlockAnimated()
{}

BlockStatic::BlockStatic(const texCoord& top, const texCoord& bot, const texCoord &side) :
	BlockTexCoord()
{
	sides = new texCoord[blockPlane::maxPlanes];
	sides[blockPlane::top] = top;
	sides[blockPlane::bottom] = bot;
	sides[blockPlane::side] = side;
}

void BlockAnimated::setStaticInstance(BlockType *bt)
{
	btype = bt;
}

