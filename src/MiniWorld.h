#ifndef MINIWORLD_QZJL4BIM
#define MINIWORLD_QZJL4BIM

//in terms of chunks
#define MINIWORLD_W 2
#define MINIWORLD_H 2
#define MINIWORLD_D 2

#include "utils/glm.h"
#include "render/Camera.h"
#include "Planet.h"
#include "Chunk.h"

#define MINIWORLD_SIZE (MINIWORLD_W*CHUNK_SIZE)

class MiniWorld
{
    public:
    	MiniWorld(Planet* p, PlanetFace* pf);

    	void draw(Camera& c);

    private:
    	Planet* planet;
    	PlanetFace* face;
    	Chunk* chunks[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D];
    	glm::mat4 model;
};

#endif
