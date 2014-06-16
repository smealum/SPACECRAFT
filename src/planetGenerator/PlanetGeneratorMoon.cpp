#include "PlanetGeneratorMoon.h"
#include "PlanetInfo.h" 
#include "MiniWorld.h"
#include "utils/dbg.h"
#include "utils/positionMath.h"


PlanetGeneratorMoon::PlanetGeneratorMoon(int nbThread):
	PlanetGenerator(nbThread)
{
}

PlanetGeneratorMoon::~PlanetGeneratorMoon()
{
}

void PlanetGeneratorMoon::generateWorldData(int threadId,
			chunkVal* data,
			int w, int h, int d, // array sizes (in chunks)
			int px, int py, int pz, // offset in world
			glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) // toplevelCharacteristic
{

}

PlanetGeneratorResponse PlanetGeneratorMoon::getCharacteristic(int threadId, const glm::vec3& pos)
{
	return {planetInfo->waterLevelElevation,blockTypes::sand};
}

float PlanetGeneratorMoon::getElevation(int threadId, const glm::vec3 &coord)
{
	return planetInfo->waterLevelElevation;
}
