#include "MiniWorld.h"

MiniWorld::MiniWorld(Planet* p, PlanetFace* pf):
	planet(p),
	model(glm::mat4(1.0f)),
	face(pf)
{
	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_H;j++)
		{
			for(int k=0;k<MINIWORLD_D;k++)
			{
				chunks[i][j][k]=new Chunk(p,this,
						(pf->x*MINIWORLD_W+i)*CHUNK_N,
						j*CHUNK_N,
						(pf->z*MINIWORLD_D+k)*CHUNK_N,
						face->toplevel->uvertex[1]-face->toplevel->uvertex[0],
						face->toplevel->uvertex[3]-face->toplevel->uvertex[0],
						face->toplevel->uvertex[0]);
			}
		}
	}
}

MiniWorld::~MiniWorld()
{
	for(int i=0;i<MINIWORLD_W;i++)
		for(int j=0;j<MINIWORLD_H;j++)
			for(int k=0;k<MINIWORLD_D;k++)
				if(chunks[i][j][k])chunks[i][j][k]->destroyChunk();
}

void MiniWorld::draw(Camera& c)
{
	//TODO : frustum culling !
	//TODO : occlusion culling ?

	// model=glm::scale(glm::mat4(1.f),glm::vec3(1.f/MINIWORLD_SIZE));
	// model=glm::translate(glm::mat4(1.f),face->vertex[0]*face->elevation)*model;

	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_H;j++)
		{
			for(int k=0;k<MINIWORLD_D;k++)
			{
				chunks[i][j][k]->draw(c, model);
			}
		}
	}
}
