#include "PlanetGenerator.h"
#include "PlanetInfo.h" 
#include "utils/dbg.h"

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

float PlanetGenerator::generateWorldData(int threadId, const chunkVal* data,
			int w, int h, int d, // array sizes (in chunks)
			int px, int py, int pz, // offset in world
			glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) const  // toplevelCharacteristic
{
	return 0.0;
}

float PlanetGenerator::getElevation(int threadId, const glm::vec3 &coord) const
{
	return generators[threadId]->getElevation(coord);
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
