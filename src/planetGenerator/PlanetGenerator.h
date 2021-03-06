#ifndef PLANETGENERATOR_PJW8VJTB
#define PLANETGENERATOR_PJW8VJTB

#include "world/BlockType.h"
#include "Chunk.h"


struct PlanetGeneratorResponse
{
	float elevation;	
	blockTypes::T tile;
};


class PlanetInfo;

class PlanetGenerator
{
	public:
		PlanetGenerator(int nbThread);
		virtual void setPlanetInfo(PlanetInfo* p);

		// genere un miniworld
		virtual void generateWorldData(int threadId,chunkVal* data,
					int w, int h, int d, // array sizes (in chunks)
					int px, int py, int pz, // offset in world
					glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) = 0; // toplevelCharacteristic

		// genere l'elevation,texture.
		virtual PlanetGeneratorResponse getCharacteristic(int threadId, const glm::vec3& pos)  = 0;

		// genere seulement l'elevation
		virtual float getElevation(int threadId, const glm::vec3& pos)  = 0;

	protected:
		int nbThread;
		PlanetInfo* planetInfo;

		void putBlock(chunkVal* data,
					int x, int y, int z, // position of the bloc dans le miniworld
					blockTypes::T b
		);

};
#endif /* end of include guard: PLANETGENERATOR_PJW8VJTB */
