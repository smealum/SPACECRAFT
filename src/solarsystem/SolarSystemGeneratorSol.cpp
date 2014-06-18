#include "solarsystem/SolarSystemGeneratorSol.h"
#include "planetGenerator/PlanetGeneratorEarth.h"
#include "utils/mt19937.h"
#include "utils/dbg.h"


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
		int nseed=seed*1000+i*10;
		init_genrand64(nseed);
		const int numSatellites=genrand64_int64()&3;
		double distance=genrand64_real2()*1e3+1e2;

		SpaceObjectTrajectory* trajectory =
						new EllipticalTrajectory(
							glm::vec3(0.0f),
							glm::mat3(distance),
							i*1.037f,
							100.0f*(i+1));

		v.push_back(new PlanetInfoEarth(
						trajectory,
						contentHandler,
						nseed,
						1)
					);

		for(int j=0;j<numSatellites;j++)
		{
			int nseed=seed*1000+i*10+j+1;
			init_genrand64(nseed);
			double satdistance=genrand64_real2()*2e1+3e0;
			double satperiod=genrand64_real2()*1e1+7e0;
			// int satsize=genrand64_int64()&1+2;
			int satsize=3;

			SpaceObjectTrajectory* satTrajectory =
							new EllipticalTrajectory(
								*trajectory,
								glm::mat3(satdistance),
								genrand64_real2()*2,
								satperiod);

			v.push_back(new PlanetInfoMoon(
							satTrajectory,
							contentHandler,
							nseed,
							satsize)
						);
		}
	}
}
