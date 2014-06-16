#include "PlanetInfo.h"
#include "MiniWorld.h"
#include "planetGenerator/PlanetGenerator.h"
#include "utils/positionMath.h"
#include "utils/dbg.h"

//setPlanetInfo is the responsibility of descending classes as it should only be called after PI's been fully initialized
PlanetInfo::PlanetInfo(SpaceObjectTrajectory* t, PlanetGenerator* p, int seed, int size):
	seed(seed),
	trajectory(t),
	axis(glm::normalize(glm::vec3(1.0,1.0,1.0))),
	period(1.0f),
	planetGenerator(p),
	size(size),
	waterLevelBlock(float(CHUNK_N*MINIWORLD_H)*0.5),
	numBlocks(PLANETFACE_BLOCKS>>(size-1))
{
	waterLevelElevation=(dblockHeightToElevation(waterLevelBlock, numBlocks));
}

//la suppression de trajectory n'est PAS la responsabilité de planetInfo
PlanetInfo::~PlanetInfo()
{
}

PlanetInfoEarth::PlanetInfoEarth(SpaceObjectTrajectory* t, PlanetGenerator* p, int seed, int size):
	PlanetInfo(t,p,seed,size),
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
	riverDepth(0.0223f)
{
	planetGenerator->setPlanetInfo(this);
}

//la suppression de trajectory n'est PAS la responsabilité de planetInfo
PlanetInfoEarth::~PlanetInfoEarth()
{
}

PlanetInfoMoon::PlanetInfoMoon(SpaceObjectTrajectory* t, PlanetGenerator* p, int seed, int size):
	PlanetInfo(t,p,seed,size)
{
	planetGenerator->setPlanetInfo(this);
}

//la suppression de trajectory n'est PAS la responsabilité de planetInfo
PlanetInfoMoon::~PlanetInfoMoon()
{
}
