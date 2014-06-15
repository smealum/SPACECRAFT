#ifndef __PLANETINFO_H__
#define __PLANETINFO_H__

#include "SpaceObjectTrajectory.h"
#include "render/Atmosphere.h"

class PlanetGenerator;

class PlanetInfo
{
	public:
		PlanetInfo(SpaceObjectTrajectory* t, PlanetGenerator* planetGen, int seed, int size=1);
		~PlanetInfo();

		int seed;
		int size;

		AtmosphereInfo atmosphereInfo;
		PlanetGenerator* planetGenerator;
		SpaceObjectTrajectory* trajectory;
		glm::vec3 axis;
		float period;
		int numBlocks;		
};

class PlanetInfoEarth : public PlanetInfo
{
	public:
		PlanetInfoEarth(SpaceObjectTrajectory* t, PlanetGenerator* planetGen, int seed, int size=1);
		~PlanetInfoEarth();

		float continentFrequency,
				continentLacunarity,
				mountainLacunarity,
				hillsLacunarity,
				plainsLacunarity,
				badlandsLacunarity,
				mountainsTwist,
				hillsTwist,
				badlandsTwist,
				seaLevel,
				shelfLevel,
				mountainsAmount,
				hillsAmount,
				badlandsAmount,
				terrainOffset,
				mountainGlaciation,
				continentHeightScale,
				riverDepth;		
};

#endif
