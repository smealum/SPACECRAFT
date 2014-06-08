#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include "Planet.h"
#include "render/Sun.h"
#include "render/Camera.h"

class SolarSystem
{
	friend class SolarSystemDataRequest;
	public:
		SolarSystem(ContentHandler& ch);
		~SolarSystem();

		void draw(Camera& c);
		void update(float delta);

		Planet* getClosestPlanet(glm::vec3 p);
		
		void deleteSolarSystem(void);

		TrackerPointer<SolarSystem>* getTptr(void);

	private:
		bool generated;

		int numPlanets;

		Sun* sun;
		Planet** planets;

		ContentHandler& contentHandler;
		TrackerPointer<SolarSystem>* tptr;
};

#endif