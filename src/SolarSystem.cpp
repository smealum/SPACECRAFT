#include "SolarSystem.h"
#include "data/ContentHandler.h"

SolarSystem::SolarSystem(ContentHandler& ch):
	generated(false),
	tptr(new TrackerPointer<SolarSystem>(this, true)),
	contentHandler(ch)
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

#include "utils/Input.h" //TEMP

void SolarSystem::update(float delta)
{
	if(!generated)return;

	// sunPosition
	{
		static float testAngle = 0.0;
		if (Input::isKeyHold(GLFW_KEY_P))	testAngle+=0.8f*delta;
		if (Input::isKeyHold(GLFW_KEY_M))	testAngle-=0.8f*delta;
		glm::vec4 sunPosition(EARTH_SUN,0.0,0.0,1.0);
		sunPosition = glm::rotate(glm::mat4(1.0),testAngle,glm::vec3(0.0,1.0,0.0)) * sunPosition;
		sun->setPosition(glm::vec3(sunPosition));

		for(int i=0;i<numPlanets;i++)
		{
			planets[i]->setSunPosition(glm::vec3(sunPosition));
		}
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
