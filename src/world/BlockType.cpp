#include "world/BlockType.h"
#include "utils/dbg.h"

const float BlockAnimated::frameTime(0.5f);
BlockType *BlockTexCoord::btype(NULL);
std::list<BlockAnimated*> BlockAnimated::list;

blockTransparency::T getTextureTransparency(const unsigned char *img, int xx, int yy, int w, int h, int texWidth)
{
	blockTransparency::T t = blockTransparency::opaque;
	for (int y = yy; y < yy+h; ++y) {
		for(int x = xx; x < xx+w; ++x)
		{
			unsigned char alpha = img[(x + y * texWidth)*4 + 3];
			if (alpha > 3 && alpha < 255)
				return blockTransparency::seeThrough;
			if (t == blockTransparency::opaque && alpha < 3)
				t = blockTransparency::transparent;
		}
	}
	return t;
}

BlockType::BlockType() :
	texWidth(0),
	texHeight(0),
	texCols(TEXCOLS) // XXX must be set manually
{
	TextureManager::getInstance().getTextureSize("data/blocksPack.png", &texWidth, &texHeight);
	TextureManager::getInstance().bind("data/blocksPack.png");
	unsigned char *img = new unsigned char[texWidth*texHeight*4];

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

	// fill texcoord map
	int blockSize = texWidth / texCols,
		texRows = texHeight / blockSize;

	debug("%u texcoord will be generated", blockTypes::maxTypeValue);
	// special air:
	blocks[0] =  new BlockStatic({
			blockTypes::air,
			blockTypes::air,
			blockTypes::air
			});
	blocks[0]->setTransparency(blockTransparency::invisible);
	texCoords[0] = glm::vec2(0.f);
	for (uint32_t i = 1; i < blockTypes::maxTypeValue; i++)
	{
		blockTypes::T t = (blockTypes::T)i;
		auto &coord = texCoords[t];
		auto &block = blocks[t];

		block = NULL;
		switch (t) {
			case blockTypes::grass:
				block = new BlockStatic({
							blockTypes::grass,
							blockTypes::grass_side,
							blockTypes::dirt
							});
				break;
			case blockTypes::water:
				block = new BlockAnimated({
						blockTypes::water,
						blockTypes::water_1,
						blockTypes::water_2,
						blockTypes::water_3,
						blockTypes::water_4
						});
				break;
			default:
				block = new BlockStatic({t,t,t});
				break;
		}
		
		//pour set le style indépendament
		switch (t) {
			case blockTypes::flower_red:
				block->setStyle(blockStyle::sprite);
				break;
			default:
				block->setStyle(blockStyle::normal);
				break;
		}

		if (block)
			block->setTransparency(
					getTextureTransparency(img,
						((i-1) % texCols) * blockSize,
						((i-1) / texCols) * blockSize,
						blockSize, blockSize, texWidth)
					);

		glm::vec2 v(
				((i-1) % texCols) / (float)texCols,
				((i-1) / texCols) / (float)texRows
				);

		coord = v;

		//debug("Added texCoord for type %d", i);
	}

	delete[] img; // free the texture
	BlockTexCoord::setStaticInstance(this);
}

BlockType::~BlockType()
{
	// free the blocktypes
	for (uint32_t i = 1; i < blockTypes::maxTypeValue; ++i)
		delete blocks[i];
}

BlockAnimated::BlockAnimated(std::initializer_list<blockTypes::T> frames) :
	BlockTexCoord(),
	frames(frames),
	current(0),
	timer(0.f)
{
	// sides remain unused
	list.push_back(this);
}

void BlockAnimated::animate(float delta)
{
	timer += delta;
	while (timer > frameTime)
		timer -= frameTime, current++;
	if (current >= frames.size())
		current -= frames.size();
	//debug("current: %u, blocktype: %u", current, frames[current]);
}

BlockAnimated::~BlockAnimated()
{
	list.remove(this);
}

BlockStatic::BlockStatic(std::initializer_list<blockTypes::T> sdes) :
	BlockTexCoord(),
	sides(sdes)
{
}

void BlockTexCoord::setStaticInstance(BlockType *bt)
{
	btype = bt;
}

void BlockAnimated::animation(float delta)
{
	for (auto it(list.begin()); it != list.end(); ++it)
		(*it)->animate(delta);
}

texCoord BlockAnimated::getSide(blockPlane::T) const
{
	return btype->getTexcoord(frames[current]);
}

texCoord BlockStatic::getSide(blockPlane::T p) const
{
	return btype->getTexcoord(sides[p]);
}

bool BlockType::shouldBeFace(blockTypes::T type1, blockTypes::T type2)
{
	const BlockTexCoord* b1=blocks[type1];
	const BlockTexCoord* b2=blocks[type2];

	return b1->getStyle()==blockStyle::normal && type1!=type2 && (b1->getTransparency()<=blockTransparency::transparent && b2->getTransparency()>=blockTransparency::seeThrough);
}

