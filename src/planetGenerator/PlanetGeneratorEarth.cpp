#include "PlanetGeneratorEarth.h"
#include "PlanetInfo.h" 
#include "MiniWorld.h"
#include "utils/dbg.h"
#include "utils/positionMath.h"

PlanetGeneratorEarth::PlanetGeneratorEarth(int nbThread):
	PlanetGenerator(nbThread)
{
}

void PlanetGeneratorEarth::initGenerators()
{
	for(int i=0;i<nbThread;++i)
		generators.push_back(new PlanetNoiseGenerator(*planetInfo));
}

PlanetGeneratorEarth::~PlanetGeneratorEarth()
{

}


float PlanetGeneratorEarth::generateWorldData(int threadId,
			const chunkVal* data,
			int w, int h, int d, // array sizes (in chunks)
			int px, int py, int pz, // offset in world
			glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) const // toplevelCharacteristic
{
	return 0.0;
}

inline double max(double x, double y)
{
	return (x>y)? x : y;
}

PlanetGeneratorResponse PlanetGeneratorEarth::getCharacteristic(int threadId, const glm::vec3& pos) const
{
	glm::vec3 posn = glm::normalize(pos);
	float elevation = getElevation(threadId,posn);
	double block_height=delevationToBlockHeight(elevation);

	float temperature = getTemperature(posn);
	float humidity = getHumidity(posn);
	if (block_height>double(CHUNK_N*MINIWORLD_H)*0.5) //  terre
	{
		// e > 0
		float e = (elevation - 1.001) * 1000.f ;

		float sandCoef = 4.0*temperature + 1.4*e - humidity;
		float snowCoef = -2.0*temperature+ 1.4*e + 0.3*humidity;
		float stoneCoef = snowCoef+0.01 - 0.1*humidity;
		float grassCoef = 0.05 + 2.0*abs(humidity);

		// inihibition
		// (pas de sable près de l'eau)
		if (e<0.05) sandCoef = 0.0;
		// (A partir d'un moment la neige recouvre les cailloux
		stoneCoef = max(stoneCoef,0.8);

	
		// on choisit le plus grand
		int imax=0;int vmax=sandCoef;
		if (vmax<snowCoef) {vmax=snowCoef;imax=1;}
		if (vmax<stoneCoef) {vmax=stoneCoef;imax=2;}
		if (vmax<grassCoef) {vmax=grassCoef;imax=3;}
		
		
		switch(imax)
		{
			case 0: return {elevation,blockTypes::sand};
			case 1: return {elevation,blockTypes::snow};
			case 2: return {elevation,blockTypes::stone};
			case 3: return {elevation,blockTypes::grass};
		}

	}
	else // mer
	{
		return {elevation,blockTypes::water};
	}
}

float PlanetGeneratorEarth::getTemperature(const glm::vec3& pos) const
{
	float distanceToEquatorFactor = 1.0-2.0*abs(
			glm::dot(
				glm::normalize(pos),
				glm::normalize(planetInfo->axis)
		 ));
	float noise1 = glm::simplex(pos*10.f) * 0.5;
	float noise2 = glm::simplex(pos*100.f) * 0.05;

	return glm::clamp(distanceToEquatorFactor+noise1+noise2,-1.f,1.f);
}

float PlanetGeneratorEarth::getHumidity(const glm::vec3& pos) const
{
	float noise = glm::simplex(pos*2.f) *  1.4;
	
	return glm::clamp(noise,-1.f,1.f);
}
