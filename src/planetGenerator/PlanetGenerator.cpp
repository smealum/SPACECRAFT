#include "PlanetGenerator.h"
#include "PlanetInfo.h" 
#include "utils/dbg.h"
#include "utils/positionMath.h"

PlanetGenerator::PlanetGenerator(int n):
	nbThread(n)
{

}

PlanetGenerator::~PlanetGenerator()
{

}

void PlanetGenerator::setPlanetInfo(PlanetInfo* p)
{
	planetInfo = p;
	initGenerators();
}

void PlanetGenerator::generateWorldData(int threadId,chunkVal* data,
			int w, int h, int d, // array sizes (in chunks)
			int px, int py, int pz, // offset in world
			glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) const  // toplevelCharacteristic
{
}

float PlanetGenerator::getElevation(int threadId, const glm::vec3 &coord) const
{
	float elevation = (generators[threadId]->getElevation(glm::normalize(coord))+1.0)/2.0f;
	return blockHeightToElevation(elevation*float(CHUNK_N*MINIWORLD_H));
}

PlanetGeneratorResponse PlanetGenerator::getCharacteristic(int threadID , const glm::vec3& pos) const
{
	return {1.0,blockTypes::grass};
}

float PlanetGenerator::getTemperature(const glm::vec3& pos) const
{
	return 0.0;
}

float PlanetGenerator::getHumidity(const glm::vec3& pos) const
{
	return 0.0;
}

void PlanetGenerator::initGenerators()
{

}
