#ifndef PLANET_H
#define PLANET_H

#include <string>
#include <vector>
#include <list>
#include "utils/glm.h"
#include "utils/TrackerPointer.h"
#include "noise/PlanetNoiseGenerator.h"
#include "render/Shader.h"
#include "render/Camera.h"
#include "render/Cloud.h"
#include "world/BlockType.h"
#include "PlanetInfo.h"
#include "render/Atmosphere.h"

#define PLANET_ADDED_DETAIL (4)
// #define PFBH_MAXSIZE (1024*16)
#define PFBH_MAXSIZE (1024*512)
#define PFBH_MINCAP (128)
#define PFBH_THRESHOLD (1024)
#define PFBH_LOWTHRESHOLD (PFBH_THRESHOLD/4)

typedef struct
{
	float pos[3];
	float elevation;
	float minElevation;
	float size;
	int topTile;
	int sideTile;
	float repeat;
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
		PlanetFaceBufferHandler(PlanetFace& pf, int ms, glm::vec3 v1, glm::vec3 v2, int index=0, float alpha=1.0, bool water=false);
		~PlanetFaceBufferHandler();
	
		void addFace(PlanetFace* pf);
		void addFace(PlanetFace* pf, float elevation, blockTypes::T tile);
		void deleteFace(PlanetFace* pf);
		void draw(Camera& c, glm::vec3 lightdir);
		void resizeVBO(void);

		int getSize(void);

	private:
		int index;
		ShaderProgram &shader;
		PlanetFace& planetFace;
		std::vector<PlanetFace*> faces;
		std::vector<faceBufferEntry_s> buffer;
		int maxSize, curSize, curCapacity;
		GLuint vbo, vao;
		glm::vec3 v1, v2;
		float alpha;
};

class PlanetFace
{
	friend class PlanetFaceBufferHandler;
	friend class MiniWorld;
	friend class Chunk;
	public:
		PlanetFace(Planet* planet, glm::vec3 v[4], uint8_t id, int size=1);
		PlanetFace(Planet* planet, PlanetFace* father, uint8_t id);
		~PlanetFace();
		
		void deletePlanetFace(PlanetFaceBufferHandler* b, PlanetFaceBufferHandler* w);
		// mise a jour de l'elevation, de la température et de l'humidité
		void updateElevation(float e, blockTypes::T tile);
		bool shouldHaveMiniworld(Camera& c);
		bool isDetailedEnough(Camera& c);
		void createMiniWorld(void);
		void removeMiniWorld(void);

		void processLevelOfDetail(Camera& c, PlanetFaceBufferHandler* b, PlanetFaceBufferHandler* w);
		void draw(Camera& c, glm::vec3 lightdir, bool water=false);

		glm::vec3 getOrigin(void);
		glm::vec3 getV1(void);
		glm::vec3 getV2(void);
		glm::vec3 getN(void);

		PlanetFace* getTopLevel(void);
		int getID(void);

		TrackerPointer<PlanetFace>* getTptr(void);

		//TEMP
		void testFullGeneration(int depth, PlanetFaceBufferHandler* b);

	private:
		void finalize(void);

		Planet* planet;
		PlanetFace* father; //father == NULL <=> toplevel face
		PlanetFace* toplevel;
		PlanetFace* sons[4];

		class MiniWorld* miniworld;

		glm::vec3 box[8];
		glm::vec3 vertex[9];
		glm::vec3 uvertex[9];

		TrackerPointer<PlanetFace>* tptr;

		PlanetFaceBufferHandler* faceBuffer;
		PlanetFaceBufferHandler* waterBuffer;

		bool noBuffer;
		bool elevated;
		int x, z;
		int bufferID[2];
		float elevation;
		blockTypes::T tile;
		float minElevation;
		
		uint8_t id;
		int depth, size;
		int childrenDepth;
		inline bool isDrawingFace() {return bufferID[0]>=0;}
		bool isDisplayOk;
};

class Planet
{
	friend class PlanetFaceBufferHandler;
	friend class PlanetFace;
	friend class Chunk;
	public:
		Planet(PlanetInfo *pi, class ContentHandler& ch, std::string name);
		~Planet(); // TODO faire tous les free
		
		void processLevelOfDetail(Camera& c);
		void draw(Camera& c, bool atmo=false);
		void update(float time);

		int numMiniWorlds(void);
		void addMiniWorld(MiniWorld* mw);
		void removeMiniWorld(MiniWorld* mw);

		bool collidePoint(glm::dvec3 p, glm::dvec3 v, glm::dvec3& out);
		class Chunk* selectBlock(glm::dvec3 p, glm::dvec3 v, glm::i32vec3& out, int& dir);

		void changeBlock(glm::i32vec3 p, blockTypes::T v);
		void deleteBlock(glm::i32vec3 p);

		std::string getName(void);
		int getNumBlocks(void);

		const PlanetInfo* planetInfo; //read only
		class ContentHandler& handler;

		glm::dvec3 getGravityVector(glm::dvec3 p);
		glm::vec3 getPosition(void);
		glm::mat4 getFullModel(Camera& c);
		glm::mat3 getModel(void);
		glm::mat3 getInvModel(void);
		glm::vec3 getCameraRelativePosition(Camera& c);
		glm::dvec3 getCameraRelativeDoublePosition(Camera& c);
		PlanetFace& getTopLevelForCamera(Camera& c);
		
		void setSunPosition(glm::vec3 p);

		//TEMP
		void testFullGeneration(int depth, PlanetFaceBufferHandler* b);

	private:
		std::list<MiniWorld*> miniWorldList;
		std::string name;

		PlanetFace* faces[6];

		glm::vec3 lightdir;
		glm::vec3 sunPosition;
		glm::vec3 position;
		glm::mat3 model, invModel;

		float angle;

		float scale;
		int size;

		Cloud cloud;
		Atmosphere* atmosphere;
};

#endif
