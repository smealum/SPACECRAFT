#include "noise/CaveGenerator.h"

int main(void)
{
	CaveGenerator cave;

	// check initialization
	for (int x = 0; x < CAVE_CHUNK_SIZE_X; x++)
		for (int y = 0; y < CAVE_CHUNK_SIZE_Y; y++)
			for (int z = 0; z < CAVE_CHUNK_SIZE_Z; z++)
				assert(cave.getBlock(x, y, z));

	cave.generate();

	// check if it's digged
	bool check = false;
	for (int x = 0; x < CAVE_CHUNK_SIZE_X; x++)
		for (int y = 0; y < CAVE_CHUNK_SIZE_Y; y++)
			for (int z = 0; z < CAVE_CHUNK_SIZE_Z; z++)
				if (!cave.getBlock(x, y, z))
					check = true;
	assert(check);

	return 0;
}
