#include "data/ContentRequest.h"

//PlanetElevationRequest stuff
PlanetElevationRequest::PlanetElevationRequest(Planet& p, PlanetFace& pf, glm::vec3 c):
	planet(p),
	face(pf),
	coord(c)
{

}

void PlanetElevationRequest::process(void)
{
	elevation=glm::simplex(glm::normalize(coord));
}

void PlanetElevationRequest::update(void)
{
	face.updateElevation(elevation);
}
