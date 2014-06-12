#ifndef PLANETGENERATOREARTH_TDQAA38S
#define PLANETGENERATOREARTH_TDQAA38S

#include "PlanetGenerator.h"
#include <vector>


class PlanetGeneratorEarth : public PlanetGenerator
{
	public:
		PlanetGeneratorEarth(int nbThread);
		virtual ~PlanetGeneratorEarth();

		virtual float generateWorldData(int threadId, const chunkVal* data,
					int w, int h, int d, // array sizes (in chunks)
					int px, int py, int pz, // offset in world
					glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) const; // toplevelCharacteristic

		virtual PlanetGeneratorResponse getCharacteristic(int threadId, const glm::vec3& pos) const ;
	private:
		virtual void initGenerators();
		virtual float getTemperature(const glm::vec3& pos) const;
		virtual float getHumidity(const glm::vec3& pos) const;
};

#endif /* end of include guard: PLANETGENERATOREARTH_TDQAA38S */
