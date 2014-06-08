#include "PlanetInfo.h"


PlanetInfo::PlanetInfo(SpaceObjectTrajectory* t):
	seed(0),
	gridWidth(200),
	gridHeight(gridWidth / 2),
	continentFrequency( 1.f),
	continentLacunarity(2.089f),
	mountainLacunarity(2.142f),
	hillsLacunarity(2.162f),
	plainsLacunarity(2.314f),
	badlandsLacunarity(2.212f),
	mountainsTwist(1.f),
	hillsTwist(1.f),
	badlandsTwist(1.f),
	seaLevel(0.f),
	shelfLevel(-0.375),
	mountainsAmount(0.5f),
	hillsAmount((1.f + mountainsAmount)/2.f),
	badlandsAmount(0.03125f),
	terrainOffset(1.f),
	mountainGlaciation(1.375f),
	continentHeightScale((1.f - seaLevel) / 4.f),
	riverDepth(0.0223f),
	trajectory(t),
	period(1.0f)
{
}

//la suppression de trajectory n'est PAS la responsabilité de planetInfo
PlanetInfo::~PlanetInfo()
{
}
