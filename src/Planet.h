#ifndef PLANET_H
#define PLANET_H

#include <vector>
#include "utils/glm.h"
#include "utils/TrackerPointer.h"
#include "render/Shader.h"
#include "render/Camera.h"

typedef struct
{
	int seed;
}planetInfo_s;

typedef struct
{
	float pos[3];
	float normal[3];
}faceBufferEntry_s;

class Planet;
class PlanetFace;

// Vertex position
//      0-5-1
//      |\ /|
//      8-4-6
//      |/ \|
//      3-7-2

// Sons position
//      [0][1]
//      [2][3]

class PlanetFaceBufferHandler
{
	public:
		PlanetFaceBufferHandler(PlanetFace& pf, int ms);
		~PlanetFaceBufferHandler();
	
		void addFace(PlanetFace* pf);
		void deleteFace(PlanetFace* pf, int i);
		void changeFace(PlanetFace* pf, int i);
		void draw(Camera& c);

	private:
		ShaderProgram &shader;
		PlanetFace& planetFace;
		std::vector<PlanetFace*> faces;
		faceBufferEntry_s* buffer;
		int maxSize, curSize;
		GLuint vbo, vao;
};

class PlanetFace
{
	friend class PlanetFaceBufferHandler;
	public:
		PlanetFace(Planet* planet, glm::vec3 v[4]);
		PlanetFace(Planet* planet, PlanetFace* father, uint8_t id);
		
		void deletePlanetFace(void);
		void updateElevation(float e);
		bool isDetailedEnough(Camera& c);
		void processLevelOfDetail(Camera& c);	

		TrackerPointer<PlanetFace>* getTptr(void);

		//TEMP
		void drawDirect(void);
		void testFullGeneration(int depth, PlanetFaceBufferHandler* b);

	private:
		void finalize(void);

		Planet* planet;
		PlanetFace* father; //father == NULL <=> toplevel face
		PlanetFace* sons[4];
		glm::vec3 vertex[9];

		TrackerPointer<PlanetFace>* tptr;

		float elevation;
		uint8_t id;
		int depth;
};

class Planet
{
	public:
		Planet(planetInfo_s pi, class ContentHandler& ch);
		
		void processLevelOfDetail(Camera& c);

		const planetInfo_s planetInfo; //read only
		class ContentHandler& handler;
		
		//TEMP
		void drawDirect(void);
		void testFullGeneration(int depth, PlanetFaceBufferHandler* b);
		ShaderProgram &programBasic;

		PlanetFace* faces[6];
	private:

		//TEMP
			GLuint vaoBasic;
			GLuint vbo;
			GLuint ebo;
};

#endif
