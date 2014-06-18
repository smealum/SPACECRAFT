#ifndef SOLARSYSTEMGENERATOR_H
#define SOLARSYSTEMGENERATOR_H

#include <vector>
#include "data/ContentHandler.h"
#include "PlanetInfo.h"

class SolarSystemGenerator
{
	public:
		SolarSystemGenerator(int seed, ContentHandler& ch);

		virtual void generatePlanetInfos(std::vector<PlanetInfo*>& v)=0;
		virtual void generateSunInfo(float& size, float& color)=0;

	protected:
		int seed;
		ContentHandler& contentHandler;
};

#endif
