#ifndef PLANETGENERATOREARTH_TDQAA38S
#define PLANETGENERATOREARTH_TDQAA38S

#include "PlanetGenerator.h"


class PlanetGeneratorEarth : public PlanetGenerator
{
	public:
	PlanetGeneratorEarth();
	void setPlanetInfo(PlanetInfo* p);

	virtual float generateWorldData(const chunkVal* data,
				int w, int h, int d, // array sizes (in chunks)
				int px, int py, int pz, // offset in world
				glm::vec3 origin, glm::vec3 v1, glm::vec3 v2); // toplevelCharacteristic

	virtual PlanetGeneratorResponse getCharacteristic(const glm::vec3& pos);

	private:
	PlanetInfo* planetInfo;
	virtual float getTemperature(const glm::vec3& pos);
	virtual float getHumidity(const glm::vec3& pos);
};

#endif /* end of include guard: PLANETGENERATOREARTH_TDQAA38S */
