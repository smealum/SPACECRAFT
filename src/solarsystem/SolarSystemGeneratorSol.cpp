#include "solarsystem/SolarSystemGeneratorSol.h"
#include "planetGenerator/PlanetGeneratorEarth.h"
#include <random>


SolarSystemGeneratorSol::SolarSystemGeneratorSol(int seed, ContentHandler& ch):
	SolarSystemGenerator(seed, ch)
{

}

void SolarSystemGeneratorSol::generatePlanetInfos(std::vector<PlanetInfo*>& v)
{
	std::mt19937 engine(seed);
	std::uniform_int_distribution<int> int_dist(1,10);
	std::uniform_real_distribution<float> float_dist(0.0f,1.0f);
	std::uniform_real_distribution<double> double_distance_dist(10.0,1000.0);
		
	const int numPlanet=int_dist(engine);
	for(int i=0;i<numPlanet;i++)
	{
		double distance=double_distance_dist(engine);
		v.push_back(new PlanetInfoEarth(
						new EllipticalTrajectory(
							glm::vec3(0.0f),
							glm::mat3(distance),
							i*1.037f,
							100.0f*(i+1)
						),
						new PlanetGeneratorEarth(contentHandler.getMaxProducers()),
						seed*1000+i
					));
	}
}
