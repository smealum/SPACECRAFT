#include "data/ContentRequest.h"

//PlanetElevationRequest stuff
PlanetElevationRequest::PlanetElevationRequest(Planet& p, PlanetFace& pf, glm::vec3 c):
	planet(p),
	coord(c)
{
	face=pf.getTptr();
	face->grab();
}

PlanetElevationRequest::~PlanetElevationRequest()
{}

void PlanetElevationRequest::process(void)
{
	// elevation=1.0+glm::simplex(glm::normalize(coord))*0.1f;
	elevation=1.0+fabs(glm::simplex(glm::normalize(coord*1000.0f)))*0.1f+fabs(glm::simplex(glm::normalize(coord*100.0f)))*0.01f+glm::simplex(glm::normalize(coord*10.0f))*0.001;
}

void PlanetElevationRequest::update(void)
{
	face->getPointer()->updateElevation(elevation);
	face->release();
}
