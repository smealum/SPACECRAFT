#include "data/ContentRequest.h"
#include "world/BlockType.h"
#include "world/blockProcessing.h"
#include "MiniWorld.h"
#include "utils/dbg.h"

#include <cstdio>

using namespace glm;

//PlanetElevationRequest stuff
PlanetElevationRequest::PlanetElevationRequest(Planet& p, PlanetFace& pf, glm::vec3 c):
	coord(c),
	planet(p)
{
	face=pf.getTptr();
	face->grab();
}

PlanetElevationRequest::~PlanetElevationRequest()
{}

static inline float getElevation(int prod_id, Planet& planet, glm::vec3 v)
{
	return (planet.getElevation(prod_id, glm::normalize(v))+2.0f)/4.0f; //faut que ça nous sorte une valeur entre 0 et 1
}

void PlanetElevationRequest::process(int id)
{
	elevation=1.0f+(getElevation(id, planet, glm::normalize(coord))*CHUNK_N*MINIWORLD_H)/PLANETFACE_BLOCKS; //faudra passer par le helper hypothétique à terme, au cas où on aurait envie de changer les dimensions des blocs...
}

void PlanetElevationRequest::update(void)
{
	face->getPointer()->updateElevation(elevation);
	face->release();
}

//WorldChunkRequest stuff
WorldChunkRequest::WorldChunkRequest(Planet& p, Chunk& c, float elevation, glm::vec3 o, glm::vec3 v1, glm::vec3 v2, int x, int y, int z):
	px(x),
	py(y),
	pz(z),
	elevation(elevation),
	origin(o),
	v1(v1),
	v2(v2),
	planet(p)
{
	chunk=c.getTptr();
	chunk->grab();
}

WorldChunkRequest::~WorldChunkRequest()
{}

//TODO : optimiser et proprifier
//(on peut largement optimiser les accès à data, éviter *énormément* de multiplications)
void generateWorldData(int prod_id, Planet& planet, chunkVal* data,
		int w, int h, int d, //array sizes (in chunks)
		int px, int py, int pz, //offset in world
		glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) //toplevel characteristics
{
	/*
		i CHUNK_N
		j CHUNK_N
		k CHUNK_N
		px w
		py h
		pz d
	*/
	int pxPos,pzPos,xPos,zPos,pyPos,yPos;
	pxPos=0;
	for(int cx=0;cx<w;cx++)
	{
		pzPos=pxPos;
		const int vx=cx*(CHUNK_N+2);
		for(int cz=0;cz<d;cz++)
		{
			xPos=pzPos;
			const int vz=cz*(CHUNK_N+2);
			for(int i=0;i<(CHUNK_N+2);i++)
			{
				zPos=xPos;
				for(int k=0;k<(CHUNK_N+2);k++)
				{
					pyPos=zPos;
					const glm::vec3 pos=origin+((v1*float(vx+px+i))+(v2*float(vz+pz+k)))/float(PLANETFACE_BLOCKS);
					const int height=int(getElevation(prod_id, planet, pos)*CHUNK_N*MINIWORLD_H);

					//TEMP (pour tester)

					if(height<CHUNK_N*MINIWORLD_H/2.f+12)
					{
						//UNDER THE SEAAAAAA
						for(int cy=0;cy<h;cy++)
						{
							yPos=pyPos;
							const int vy=cy*CHUNK_N;
							for(int j=0;j<(CHUNK_N+2);j++)
							{
								if (vy+py+j <= height) data[yPos]=blockTypes::sand;
								else data[yPos]=blockTypes::air;
								yPos+=(CHUNK_N+2);
							}
							pyPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w;
						}
					}else{
						for(int cy=0;cy<h;cy++)
						{
							yPos=pyPos;
							const int vy=cy*CHUNK_N;
							for(int j=0;j<(CHUNK_N+2);j++)
							{
								if (vy+py+j == height) data[yPos]=blockTypes::grass;
								else if (vy+py+j < height) data[yPos]=blockTypes::dirt;
								else if (vy+py+j == height+1 && rand()%100 == 1) data[yPos]=blockTypes::flower_red;
								else data[yPos]=blockTypes::air;
								yPos+=(CHUNK_N+2);
							}
							pyPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w;
						}
					}


					zPos+=(CHUNK_N+2)*(CHUNK_N+2);
				}
				xPos+=1;
			}
			pzPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w*h;
		}
		pxPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*1;
	}
}

void WorldChunkRequest::process(int id)
{
	generateWorldData(id, planet, (chunkVal*)data, 1, 1, 1, px, py, pz, origin, v1, v2);
	computeChunkFaces((chunkVal*)data, 1, 1, 1, 0, 0, 0, px, py, pz, vArray);
}

void WorldChunkRequest::update(void)
{
	chunk->getPointer()->updateData((chunkVal*)data, vArray);
	chunk->release();
}

//MiniWorldDataRequest stuff
MiniWorldDataRequest::MiniWorldDataRequest(Planet& p, MiniWorld& mw, glm::vec3 o, glm::vec3 v1, glm::vec3 v2, int x, int y, int z):
	px(x),
	py(y),
	pz(z),
	origin(o),
	v1(v1),
	v2(v2),
	planet(p)
{
	miniworld=mw.getTptr();
	miniworld->grab();
}

MiniWorldDataRequest::~MiniWorldDataRequest()
{}

void MiniWorldDataRequest::process(int id)
{
	generateWorldData(id, planet, (chunkVal*)data, MINIWORLD_W, MINIWORLD_H, MINIWORLD_D, px, py, pz, origin, v1, v2);

	for(int i=0;i<MINIWORLD_W;i++)
		for(int j=0;j<MINIWORLD_H;j++)
			for(int k=0;k<MINIWORLD_D;k++)
				computeChunkFaces((chunkVal*)data, MINIWORLD_W, MINIWORLD_H, MINIWORLD_D, i, j, k, px+i*CHUNK_N, py+j*CHUNK_N, pz+k*CHUNK_N, vArray[i][j][k]);
}

void MiniWorldDataRequest::update(void)
{
	miniworld->getPointer()->updateChunks(data, vArray);
	miniworld->release();
}

//MiniWorldDeletionRequest stuff
MiniWorldDeletionRequest::MiniWorldDeletionRequest(MiniWorld& mw)
{
	miniworld=mw.getTptr();
	miniworld->grab();
}

MiniWorldDeletionRequest::~MiniWorldDeletionRequest()
{}

void MiniWorldDeletionRequest::process(int id)
{
	if(miniworld->getNumRef()>1)return;
	miniworld->release();
}

void MiniWorldDeletionRequest::update(void)
{

}
