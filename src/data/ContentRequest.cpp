#include "data/ContentRequest.h"

//PlanetElevationRequest stuff
PlanetElevationRequest::PlanetElevationRequest(Planet& p, PlanetFace& pf, glm::vec3 c):
	planet(p),
	face(pf),
	coord(c)
{

}

PlanetElevationRequest::~PlanetElevationRequest()
{}

void PlanetElevationRequest::process(void)
{
	elevation=1.0+glm::simplex(glm::normalize(coord))*0.1f;
}

void PlanetElevationRequest::update(void)
{
	face.updateElevation(elevation);
}
