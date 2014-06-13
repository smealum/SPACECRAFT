#include "SolarSystem.h"
#include "data/ContentHandler.h"

SolarSystem::SolarSystem(const glm::dvec3& pos, ContentHandler& ch):
	generated(false),
	tptr(new TrackerPointer<SolarSystem>(this, true)),
	contentHandler(ch),
	numPlanets(0),
	position(pos)
{
	ch.requestContent(new SolarSystemDataRequest(*this, ch));
}

SolarSystem::~SolarSystem()
{

}

void SolarSystem::deleteSolarSystem(void)
{
	tptr->release();
}

void SolarSystem::draw(Camera& c)
{
	if(!generated)return;

	sun->draw(c);

	for(int i=0;i<numPlanets;i++)
	{
		planets[i]->processLevelOfDetail(c);
		planets[i]->draw(c);
	}

	sun->drawGlow(c);
}

void SolarSystem::update(float time)
{
	if(!generated)return;

	for(int i=0;i<numPlanets;i++)
	{
		planets[i]->update(time);
		planets[i]->setSunPosition(sun->getPosition());
	}
}

Planet* SolarSystem::getClosestPlanet(glm::vec3 p)
{
	Planet* ret=NULL;

	if(numPlanets<=0)return ret;

	float minDist=glm::length(p-planets[0]->getPosition());
	ret=planets[0];

	for(int i=1;i<numPlanets;i++)
	{
		float dist=glm::length(p-planets[i]->getPosition());
		if(dist<minDist)
		{
			minDist=dist;
			ret=planets[i];
		}
	}

	return ret;
}

TrackerPointer<SolarSystem>* SolarSystem::getTptr(void)
{
	return tptr;
}

const glm::dvec3& SolarSystem::getPosition() const
{
	return position;
}
