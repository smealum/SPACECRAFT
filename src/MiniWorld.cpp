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
	//TODO : passer frustum culling en octree ?
	//TODO : occlusion culling ?

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
