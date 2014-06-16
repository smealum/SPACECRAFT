#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include "Planet.h"
#include "render/Sun.h"
#include "render/Camera.h"
#include "utils/glm.h"

class SolarSystem
{
	friend class SolarSystemDataRequest;
	public:
		SolarSystem(const glm::dvec3& position);
		~SolarSystem();

		void generate(ContentHandler& contentHandler);

		void draw(Camera& c);
		void update(float time);

		glm::dvec3 getPosition(void);
		Planet* getClosestPlanet(glm::vec3 p);
		
		void deleteSolarSystem(void);

		TrackerPointer<SolarSystem>* getTptr(void);

		const glm::dvec3& getPosition() const;

	private:

		glm::dvec3 position;

		bool generated;

		int numPlanets;

		Sun* sun;
		Planet** planets;

		TrackerPointer<SolarSystem>* tptr;
};

#endif
