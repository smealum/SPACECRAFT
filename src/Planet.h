#ifndef PLANET_H
#define PLANET_H

#include "utils/glm.h"

typedef struct
{
	int seed;
}planetInfo_s;

class Planet;

//0-5-1
//|\ /|
//8-4-6
//|/ \|
//3-7-2

//[0][1]
//[2][3]
class PlanetFace
{
	public:
		PlanetFace(Planet* planet, glm::vec3 v[4]);
		PlanetFace(Planet* planet, PlanetFace* father, uint8_t id);
	private:
		void finalize(void);

		Planet* planet;
		PlanetFace* father; //father == NULL <=> toplevel face
		PlanetFace* sons[4];
		glm::vec3 vertex[9];
};

class Planet
{
	public:
		Planet(planetInfo_s pi);
		const planetInfo_s planetInfo; //read only
	private:
		PlanetFace* faces[6];
};

#endif
