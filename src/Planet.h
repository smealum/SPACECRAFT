#ifndef PLANET_H
#define PLANET_H

#include "utils/glm.h"
#include <render/Shader.h>

typedef struct
{
	int seed;
}planetInfo_s;

class Planet;

// Vertex position
//      0-5-1
//      |\ /|
//      8-4-6
//      |/ \|
//      3-7-2

// Sons position
//      [0][1]
//      [2][3]
class PlanetFace
{
	public:
		PlanetFace(Planet* planet, glm::vec3 v[4]);
		PlanetFace(Planet* planet, PlanetFace* father, uint8_t id);

		void updateElevation(float e);

		//TEMP
		void drawDirect(void);
		void testFullGeneration(int depth);

	private:
		void finalize(void);

		Planet* planet;
		PlanetFace* father; //father == NULL <=> toplevel face
		PlanetFace* sons[4];
		glm::vec3 vertex[9];
		float elevation;
		int depth;
};

class Planet
{
	public:
		Planet(planetInfo_s pi, class ContentHandler& ch);

		const planetInfo_s planetInfo; //read only
		class ContentHandler& handler;
		
		//TEMP
		void drawDirect(void);
		void testFullGeneration(int depth);
		ShaderProgram &programBasic;

	private:
		PlanetFace* faces[6];

		//TEMP
			GLuint vaoBasic;
			GLuint vbo;
			GLuint ebo;
};

#endif
