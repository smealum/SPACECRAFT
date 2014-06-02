#ifndef __PLANETINFO_H__
#define __PLANETINFO_H__

typedef struct
{
	int seed = 0;
	int gridWidth = 200,
		gridHeight = gridWidth / 2;
	float continentFrequency =  1.f, // higher values makes more continents
		  continentLacunarity = 2.089f, // should be randomly around 2.0
		  mountainLacunarity = 2.142f, // the same but seems not to change
		  hillsLacunarity = 2.162f, // same
		  plainsLacunarity = 2.314f,
		  badlandsLacunarity = 2.212f,
		  mountainsTwist = 1.f, // twistiness
		  hillsTwist = 1.f,
		  badlandsTwist = 1.f,
		  seaLevel = 0.f,
		  shelfLevel = -0.375,
		  mountainsAmount = 0.5f, // [0..1]
		  hillsAmount = (1.f + mountainsAmount)/2.f, // must be less than mountainsAmount because mountains overlap hills
		  badlandsAmount = 0.03125f, // badlands overlap any other type of terrain [0..1]
		  terrainOffset = 1.f, // Offset to apply to the terrain type definition.  Low values (< 1.0) cause
							   // the rough areas to appear only at high elevations.  High values (> 2.0)
							   // cause the rough areas to appear at any elevation.  The percentage of
							   // rough areas on the planet are independent of this value.
		  mountainGlaciation = 1.375f, //close to 1 but >
		  continentHeightScale = (1.f - seaLevel) / 4.f, // Scaling to apply to the base continent elevations, in planetary elevation units
		  riverDepth = 0.0223f; // max depth of rivers
}planetInfo_s;

#endif
