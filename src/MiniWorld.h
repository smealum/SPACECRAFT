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

class MiniWorld
{
    public:
    	MiniWorld(Planet* p);

    	void draw(Camera& c);

    private:
    	Planet* planet;
    	Chunk* chunks[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D];
};

#endif
