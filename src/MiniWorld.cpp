#include "MiniWorld.h"
#include "data/ContentHandler.h"

MiniWorld::MiniWorld(Planet* p, PlanetFace* pf):
	planet(p),
	model(glm::mat4(1.0f)),
    tptr(new TrackerPointer<MiniWorld>(this, true)),
	face(pf),
	origin(face->toplevel->uvertex[0]),
	v1(face->toplevel->uvertex[1]-face->toplevel->uvertex[0]),
	v2(face->toplevel->uvertex[3]-face->toplevel->uvertex[0]),
	x(pf->x*MINIWORLD_W*CHUNK_N),
	z(pf->z*MINIWORLD_D*CHUNK_N),
	generated(false)
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
    planet->handler.requestContent(new MiniWorldDataRequest(*planet, *this, origin, v1, v2, x, 0, z));
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

void MiniWorld::destroyMiniWorld(void)
{
    tptr->release();
}

TrackerPointer<MiniWorld>* MiniWorld::getTptr(void)
{
    return tptr;
}

void MiniWorld::updateChunks(chunkVal data[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D][(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)], std::vector<GL_Vertex> va[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D])
{
	generated=true;
	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_H;j++)
		{
			for(int k=0;k<MINIWORLD_D;k++)
			{
				chunks[i][j][k]->updateData(data[i][j][k], va[i][j][k]);
			}
		}
	}
}
