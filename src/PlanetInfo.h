#ifndef __PLANETINFO_H__
#define __PLANETINFO_H__

#include "SpaceObjectTrajectory.h"

class PlanetInfo
{
	public:
		PlanetInfo(SpaceObjectTrajectory* t);
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
		float period; //rotation propre
};

#endif
