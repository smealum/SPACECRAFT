#ifndef __PLANETINFO_H__
#define __PLANETINFO_H__

class PlanetInfo
{
	public:
		PlanetInfo();

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
};

#endif
