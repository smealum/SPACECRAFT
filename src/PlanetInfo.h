#ifndef __PLANETINFO_H__
#define __PLANETINFO_H__

#include "SpaceObjectTrajectory.h"

class PlanetGenerator;
class PlanetInfo
{
	public:
		PlanetInfo(SpaceObjectTrajectory* t, PlanetGenerator* planetGen);
		~PlanetInfo();

		int seed;
		int gridWidth,
			gridHeight;
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

		SpaceObjectTrajectory* trajectory;

		// Axe de rotation propre
		glm::vec3 axis;
		float period;
		
		PlanetGenerator* planetGenerator;

};

#endif
