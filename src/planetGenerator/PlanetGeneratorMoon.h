#ifndef PLANETGENERATORMOON_H
#define PLANETGENERATORMOON_H

#include "PlanetGenerator.h"
#include <vector>


class PlanetGeneratorMoon : public PlanetGenerator
{
	public:
		PlanetGeneratorMoon(int nbThread);
		virtual ~PlanetGeneratorMoon();

		virtual void generateWorldData(int threadId,chunkVal* data,
					int w, int h, int d, // array sizes (in chunks)
					int px, int py, int pz, // offset in world
					glm::vec3 origin, glm::vec3 v1, glm::vec3 v2); // toplevelCharacteristic

		virtual PlanetGeneratorResponse getCharacteristic(int threadId, const glm::vec3& pos);

		virtual float getElevation(int threadId, const glm::vec3 &coord);
	
	private:
};

#endif
