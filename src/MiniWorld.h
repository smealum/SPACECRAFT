#ifndef MINIWORLD_QZJL4BIM
#define MINIWORLD_QZJL4BIM

//in terms of chunks
#define MINIWORLD_W 1
#define MINIWORLD_D MINIWORLD_W
#define MINIWORLD_H 32

#include "utils/glm.h"
#include "render/Camera.h"
#include "Planet.h"
#include "Chunk.h"

#define MINIWORLD_N (MINIWORLD_W*CHUNK_N)
#define MINIWORLD_SIZE (MINIWORLD_W*CHUNK_SIZE)

// #define MINIWORLD_DETAIL (13)
// #define MINIWORLD_DETAIL (11)
#define MINIWORLD_DETAIL (14)
#define PLANETFACE_BLOCKS ((1<<MINIWORLD_DETAIL)*MINIWORLD_N)

//do NOT instanciate directly
//	use new
//do NOT destroy directly
//	use destroyMiniWorld
class MiniWorld
{
    friend class Chunk;
	public:
		MiniWorld(Planet* p, PlanetFace* pf);
		~MiniWorld();

		void draw(Camera& c);
		void destroyMiniWorld(void);
		void updateChunks(chunkVal data[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D][(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)], std::vector<GL_Vertex> va[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D]);

		bool collidePoint(glm::dvec3& p, glm::dvec3& v);
		bool selectBlock(glm::dvec3 p, glm::dvec3 v, glm::i32vec3& out);

		inline bool isGenerated(void)
		{
			return generated;
		}

		TrackerPointer<MiniWorld>* getTptr(void);

	private:
		Planet* planet;
		PlanetFace* face;
		Chunk* chunks[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D];
		glm::mat4 model;
		glm::vec3 origin, v1, v2;
		int x, z;
		bool generated;

		TrackerPointer<MiniWorld>* tptr;
};

#endif
