#include "solarsystem/SolarSystemGeneratorSol.h"
#include "planetGenerator/PlanetGeneratorEarth.h"
#include "utils/mt19937.h"


SolarSystemGeneratorSol::SolarSystemGeneratorSol(int seed, ContentHandler& ch):
	SolarSystemGenerator(seed, ch)
{

}

void SolarSystemGeneratorSol::generatePlanetInfos(std::vector<PlanetInfo*>& v)
{
	init_genrand64(seed);
		
	const int numPlanet=genrand64_int64()&7;
	for(int i=0;i<numPlanet;i++)
	{
		double distance=genrand64_real2()*1e3+1e1;

		SpaceObjectTrajectory* trajectory =
						new EllipticalTrajectory(
							glm::vec3(0.0f),
							glm::mat3(distance),
							i*1.037f,
							100.0f*(i+1));

		v.push_back(new PlanetInfoEarth(
						trajectory,
						contentHandler,
						seed*1000+i*10,
						1)
					);

		const int numSatellites=genrand64_int64()&3;
		for(int j=0;j<numSatellites;j++)
		{
			SpaceObjectTrajectory* satTrajectory =
							new EllipticalTrajectory(
								*trajectory,
								glm::mat3(3.0f),
								i*1.037f,
								10.0f);

			v.push_back(new PlanetInfoMoon(
							satTrajectory,
							contentHandler,
							seed*1000+i*10+j+1,
							3)
						);
		}
	}
}
