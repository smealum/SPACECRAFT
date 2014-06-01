#ifndef MINIWORLD_QZJL4BIM
#define MINIWORLD_QZJL4BIM

//in terms of chunks
#define MINIWORLD_W 8
#define MINIWORLD_D MINIWORLD_W
#define MINIWORLD_H 8

#include "utils/glm.h"
#include "render/Camera.h"
#include "Planet.h"
#include "Chunk.h"

#define MINIWORLD_N (MINIWORLD_W*CHUNK_N)
#define MINIWORLD_SIZE (MINIWORLD_W*CHUNK_SIZE)

// #define MINIWORLD_DETAIL (13)
#define MINIWORLD_DETAIL (11)
#define PLANETFACE_BLOCKS ((1<<MINIWORLD_DETAIL)*MINIWORLD_N)

class MiniWorld
{
    friend class Chunk;
	public:
		MiniWorld(Planet* p, PlanetFace* pf);
		~MiniWorld();

		void draw(Camera& c);

	private:
		Planet* planet;
		PlanetFace* face;
		Chunk* chunks[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D];
		glm::mat4 model;
};

#endif
