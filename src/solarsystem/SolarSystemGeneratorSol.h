#ifndef SOLARSYSTEMGENERATORSOL_H
#define SOLARSYSTEMGENERATORSOL_H

#include "SolarSystemGenerator.h"

class SolarSystemGeneratorSol : public SolarSystemGenerator
{
	public:
		SolarSystemGeneratorSol(int seed, ContentHandler& ch);

		virtual void generatePlanetInfos(std::vector<PlanetInfo*>& v);
		virtual void generateSunInfo(float& size, float& color);

	private:
};

#endif
