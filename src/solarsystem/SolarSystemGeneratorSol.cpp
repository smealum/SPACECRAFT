#include "solarsystem/SolarSystemGeneratorSol.h"
#include "planetGenerator/PlanetGeneratorEarth.h"


SolarSystemGeneratorSol::SolarSystemGeneratorSol(int seed, ContentHandler& ch):
	SolarSystemGenerator(seed, ch)
{

}

void SolarSystemGeneratorSol::generatePlanetInfos(std::vector<PlanetInfo*>& v)
{
	const int numPlanet=2;

	for(int i=0;i<numPlanet;i++)
	{
		v.push_back(new PlanetInfoEarth(
						new EllipticalTrajectory(
							glm::vec3(0.0f),
							glm::mat3(10.0f*(i+1)),
							i*1.037f,
							100.0f*(i+1)
						),
						new PlanetGeneratorEarth(contentHandler.getMaxProducers()),
						seed*1000+i
					));
	}
}
