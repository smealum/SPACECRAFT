#include "MiniWorld.h"
#include "data/ContentHandler.h"
#include "utils/dbg.h"

MiniWorld::MiniWorld(Planet* p, PlanetFace* pf):
	planet(p),
	model(1.0f),
    tptr(new TrackerPointer<MiniWorld>(this, true)),
	face(pf),
	origin(face->toplevel->uvertex[0]),
	v1(face->toplevel->uvertex[1]-face->toplevel->uvertex[0]),
	v2(face->toplevel->uvertex[3]-face->toplevel->uvertex[0]),
	x(pf->x*MINIWORLD_W*CHUNK_N),
	z(pf->z*MINIWORLD_D*CHUNK_N),
	generated(false),
	modified(false),
	constructionCanceled(false)
{
	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_H;j++)
		{
			for(int k=0;k<MINIWORLD_D;k++)
			{
				chunks[i][j][k]=new Chunk(p,this,
						x+i*CHUNK_N,
						j*CHUNK_N,
						z+k*CHUNK_N,
						face->toplevel->uvertex[1]-face->toplevel->uvertex[0],
						face->toplevel->uvertex[3]-face->toplevel->uvertex[0],
						face->toplevel->uvertex[0]);
			}
		}
	}
	planet->handler.requestContent(new MiniWorldDataRequest(*planet, *this, origin, v1, v2, x, 0, z, p->getNumBlocks(), planet->handler));
}

MiniWorld::~MiniWorld()
{
	for(int i=0;i<MINIWORLD_W;i++)
		for(int j=0;j<MINIWORLD_H;j++)
			for(int k=0;k<MINIWORLD_D;k++)
				if(chunks[i][j][k])chunks[i][j][k]->destroyChunk();
}

#include <sstream>

std::string MiniWorld::getName(void)
{
	std::ostringstream oss;
	oss << planet->getName() << "_" << face->getTopLevel()->getID() << "_" << x << "_" << z;
	return oss.str();
}

void MiniWorld::draw(Camera& c)
{
	//TODO : passer frustum culling en octree ?
	//TODO : occlusion culling ?

	model=glm::translate(glm::mat4(1.0f),planet->getPosition()-c.getReference())*glm::mat4(planet->getModel());

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

bool MiniWorld::isConstructionCanceled()
{
	return constructionCanceled;
}

void MiniWorld::destroyMiniWorld(void)
{
	constructionCanceled = true;
	planet->handler.requestContent(new MiniWorldDeletionRequest(*this, planet->handler),false);
		tptr->release();
	planet->handler.manualReleaseInput();
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

bool MiniWorld::collidePoint(glm::dvec3& p, glm::dvec3& v)
{
	if(p.x<x-1 || p.z<z-1 || p.x>x+CHUNK_N*MINIWORLD_W+1 || p.z>z+CHUNK_N*MINIWORLD_D+1)return false;
	bool ret=false;
	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_H;j++)
		{
			for(int k=0;k<MINIWORLD_D;k++)
			{
				ret=chunks[i][j][k]->collidePoint(p,v)||ret;
			}
		}
	}
	return ret;
}

Chunk* MiniWorld::selectBlock(glm::dvec3 p, glm::dvec3 v, glm::i32vec3& out, glm::dvec3& out2, bool& done, int& dir)
{
	if(p.x<x-1 || p.z<z-1 || p.x>x+CHUNK_N*MINIWORLD_W+1 || p.z>z+CHUNK_N*MINIWORLD_D+1)return NULL;
	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_H;j++)
		{
			for(int k=0;k<MINIWORLD_D;k++)
			{
				done=false;
				bool ret=chunks[i][j][k]->selectBlock(p,v,out,out2,done,dir);
				if(ret)return chunks[i][j][k];
				if(done)return NULL;
			}
		}
	}
	return NULL;
}

void MiniWorld::changeBlock(glm::i32vec3 p, blockTypes::T v)
{
	if(p.x<x-1 || p.z<z-1 || p.x>x+CHUNK_N*MINIWORLD_W+1 || p.z>z+CHUNK_N*MINIWORLD_D+1)return;
	modified=true;
	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_H;j++)
		{
			for(int k=0;k<MINIWORLD_D;k++)
			{
				chunks[i][j][k]->changeBlock(p,v);
			}
		}
	}
}

void MiniWorld::deleteBlock(glm::i32vec3 p)
{
	changeBlock(p,blockTypes::air);
}
