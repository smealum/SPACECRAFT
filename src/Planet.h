#ifndef PLANET_H
#define PLANET_H

#include <vector>
#include <list>
#include "utils/glm.h"
#include "utils/TrackerPointer.h"
#include "render/Shader.h"
#include "render/Camera.h"
#include "PlanetInfo.h"
#include "noise/PlanetGenerator.h"

#define PLANET_MAXDETAIL (16)

typedef struct
{
	float pos[3];
	float elevation;
	float minElevation;
	float size;
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
		PlanetFaceBufferHandler(PlanetFace& pf, int ms, glm::vec3 v1, glm::vec3 v2);
		~PlanetFaceBufferHandler();
	
		void addFace(PlanetFace* pf);
		void deleteFace(PlanetFace* pf);
		void changeFace(PlanetFace* pf, int i);
		void draw(Camera& c);

	private:
		ShaderProgram &shader;
		PlanetFace& planetFace;
		std::vector<PlanetFace*> faces;
		faceBufferEntry_s* buffer;
		int maxSize, curSize;
		GLuint vbo, vao;
		glm::vec3 v1, v2;
};

class PlanetFace
{
	friend class PlanetFaceBufferHandler;
	friend class MiniWorld;
	friend class Chunk;
	public:
		PlanetFace(Planet* planet, glm::vec3 v[4]);
		PlanetFace(Planet* planet, PlanetFace* father, uint8_t id);
		~PlanetFace();
		
		void deletePlanetFace(PlanetFaceBufferHandler* b);
		void updateElevation(float e);
		bool shouldHaveMiniworld(Camera& c);
		bool isDetailedEnough(Camera& c);
		void processLevelOfDetail(Camera& c, PlanetFaceBufferHandler* b);
		void createMiniWorld(void);
		void removeMiniWorld(void);

		TrackerPointer<PlanetFace>* getTptr(void);

		//TEMP
		void drawDirect(void);
		void testFullGeneration(int depth, PlanetFaceBufferHandler* b);

	private:
		void finalize(void);

		Planet* planet;
		PlanetFace* father; //father == NULL <=> toplevel face
		PlanetFace* toplevel;
		PlanetFace* sons[4];

		class MiniWorld* miniworld;

		glm::vec3 vertex[9];
		glm::vec3 uvertex[9];

		TrackerPointer<PlanetFace>* tptr;

		bool elevated;
		int x, z;
		int bufferID;
		float elevation;
		float minElevation;
		uint8_t id;
		int depth;
};

class Planet
{
	public:
		Planet(planetInfo_s &pi, class ContentHandler& ch);
		~Planet(); // TODO faire tous les free
		
		void processLevelOfDetail(Camera& c);
		void draw(Camera& c);

		int numMiniWorlds(void);
		void addMiniWorld(MiniWorld* mw);
		void removeMiniWorld(MiniWorld* mw);

		const planetInfo_s planetInfo; //read only
		class ContentHandler& handler;
		
		//TEMP
		void drawDirect(void);
		void testFullGeneration(int depth, PlanetFaceBufferHandler* b);
		ShaderProgram &programBasic;

		PlanetFace* faces[6];
		PlanetFaceBufferHandler* faceBuffers[6];

		inline float getElevation(const glm::vec3 &coord) { return generators[0]->getElevation(coord); }
	private:
		std::vector<PlanetGenerator*> generators;
		std::list<MiniWorld*> miniWorldList;

		//TEMP
			GLuint vaoBasic;
			GLuint vbo;
			GLuint ebo;
};

#endif
