#include "PlanetGeneratorEarth.h"
#include "PlanetInfo.h" 

PlanetGeneratorEarth::PlanetGeneratorEarth()
{

}

void PlanetGeneratorEarth::setPlanetInfo(PlanetInfo* p)
{
	planetInfo = p;
}

float PlanetGeneratorEarth::generateWorldData(const chunkVal* data,
			int w, int h, int d, // array sizes (in chunks)
			int px, int py, int pz, // offset in world
			glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) // toplevelCharacteristic
{
	return 0.0;
}

PlanetGeneratorResponse PlanetGeneratorEarth::getCharacteristic(const glm::vec3& pos)
{
	return {0.0,blockTypes::grass};
}

float PlanetGeneratorEarth::getTemperature(const glm::vec3& pos)
{
	return 0.0;
}

float PlanetGeneratorEarth::getHumidity(const glm::vec3& pos)
{
	return 0.0;
}

