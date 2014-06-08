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
}

void SolarSystem::update(float delta)
{
	if(!generated)return;
}

TrackerPointer<SolarSystem>* SolarSystem::getTptr(void)
{
	return tptr;
}
