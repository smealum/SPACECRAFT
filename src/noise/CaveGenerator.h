#ifndef __CAVEGENERATOR_H__
#define __CAVEGENERATOR_H__

#include "utils/glm.h"
#include <noise/noise.h>
#include "noise/noiseutils.h"

// fill a 3D grid with aire and blocks (bool) and eventually with blocktypes

typedef bool caveBlock; // easier to change in the future

#define CAVE_CHUNK_SIZE_X 64 // size of the block where the worms can travel
#define CAVE_CHUNK_SIZE_Y 64
#define CAVE_CHUNK_SIZE_Z 64
// if vertically the value is larger do it here
#define CAVE_BLOCK_SIZE CAVE_CHUNK_SIZE_X * CAVE_CHUNK_SIZE_Y * CAVE_CHUNK_SIZE_Z

class CaveGenerator {
	private:
		int seed;
		noise::module::Perlin posNoise, // density to generate starting points
							  rotNoise[3]; // one for each axis
		int segmentCount,
			segmentLength; // in units (meters?)
		float twistiness; // how much caves twist their way
		caveBlock blocks[CAVE_BLOCK_SIZE];

		// dig out a disk in the blocks array
		// normal is the up vector => sin(∆) and dir is the side one => cos(∆)
		void digDisk(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec3 &dir);
		// dig a line, checks if position are valid
		void digLine(const glm::i32vec3 &a, const glm::i32vec3 &b);

		bool isGenerated;
	public:
		CaveGenerator();
		~CaveGenerator();

		// must be called manually to generate the cave using current seed and given conf
		void generate();

		caveBlock getBlock(const glm::i32vec3 &p);

		inline caveBlock getBlock(int x, int y, int z)
		{
			int xx = (x%(2*CAVE_CHUNK_SIZE_X)); if (xx>=CAVE_CHUNK_SIZE_X) xx=2*CAVE_CHUNK_SIZE_X-xx-1;
			int yy = (y%(2*CAVE_CHUNK_SIZE_Y)); if (yy>=CAVE_CHUNK_SIZE_Y) yy=2*CAVE_CHUNK_SIZE_Y-yy-1;
			int zz = (z%(2*CAVE_CHUNK_SIZE_Z)); if (zz>=CAVE_CHUNK_SIZE_Z) zz=2*CAVE_CHUNK_SIZE_Z-zz-1;
			return blocks[
				xx + CAVE_CHUNK_SIZE_X * (
				yy + CAVE_CHUNK_SIZE_Y * (
				zz ))];
		}
};

#endif
