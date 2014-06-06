#include "world/BlockType.h"
#include "utils/dbg.h"

int blockTileID[blockTypes::max][blockPlane::max];
uint8_t blockTransparencyID[blockTypes::max];
uint8_t blockStyleID[blockTypes::max];

void blockTypeLoadValues()
{
	//////////////////////////
	// valeurs par défaut  //
	////////////////////////
	
	for(uint32_t i = 1; i < blockTypes::max; i++)
	for(uint8_t j = 0 ; j<blockPlane::max; j++)
		blockTileID[i][j] = i;
	
	for (uint32_t i = 1; i < blockTypes::max; i++)
		blockTransparencyID[i] = blockTransparency::opaque;

	for (uint32_t i = 1; i < blockStyle::max; i++)
		blockStyleID[i] = blockStyle::normal;


	////////////////////
	// modifications //
	//////////////////
	
	// air
	blockTransparencyID[blockTypes::air] = blockTransparency::invisible;
	
	// grass
	blockTileID[blockTypes::grass][blockPlane::side]   = blockTypes::grass_side;
	blockTileID[blockTypes::grass][blockPlane::bottom] = blockTypes::dirt;
	
	// flower yellow
	blockTransparencyID[blockTypes::flower_yellow] = blockTransparency::seeThrough;
	blockStyleID[blockTypes::flower_yellow] = blockStyle::sprite;

	// flower_red
	blockTransparencyID[blockTypes::flower_red] = blockTransparency::seeThrough;
	blockStyleID[blockTypes::flower_red] = blockStyle::sprite;
}

bool blockShouldBeFace(int type1, int type2)
{
	return
		type1 != type2 &&
		blockStyleID[type1] == blockStyle::normal &&
		blockTransparencyID[type1] <= blockTransparency::transparent &&
		blockTransparencyID[type2] >= blockTransparency::seeThrough;
}


// TODO réintégré ce code
//blockTransparency::T getTextureTransparency(const unsigned char *img, int xx, int yy, int w, int h, int texWidth)
//{
	//blockTransparency::T t = blockTransparency::opaque;
	//for (int y = yy; y < yy+h; ++y) {
		//for(int x = xx; x < xx+w; ++x)
		//{
			//unsigned char alpha = img[(x + y * texWidth)*4 + 3];
			//if (alpha > 3 && alpha < 255)
				//return blockTransparency::seeThrough;
			//if (t == blockTransparency::opaque && alpha < 3)
				//t = blockTransparency::transparent;
		//}
	//}
	//return t;
//}
