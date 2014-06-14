#include "PlanetGenerator.h"
#include "PlanetInfo.h" 
#include "utils/dbg.h"
#include "utils/positionMath.h"

PlanetGenerator::PlanetGenerator(int n):
	nbThread(n)
{
}

void PlanetGenerator::setPlanetInfo(PlanetInfo* p)
{
	planetInfo = p;
}
