#include "data/ContentRequest.h"
#include "data/ContentHandler.h"
#include "world/BlockType.h"
#include "world/blockProcessing.h"
#include "MiniWorld.h"
#include "utils/positionMath.h"
#include "utils/dbg.h"
#include "glm/gtc/noise.hpp"

#include "noise/CaveGenerator.h" // XXX debug
#include "planetGenerator/PlanetGeneratorEarth.h"

#include <cstdio>

using namespace glm;

// ContentRequest
ContentRequest::ContentRequest():
	isCanceled(false)
{
}

bool ContentRequest::isRelevant(int id)
{
	return true;
}

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


void PlanetElevationRequest::process(int id)
{
	glm::vec3 pos = glm::normalize(coord);
	PlanetGeneratorResponse p = planet.planetInfo->planetGenerator->getCharacteristic(id,pos);
	elevation = p.elevation;
	tile = p.tile;
}

void PlanetElevationRequest::update(void)
{
	face->getPointer()->updateElevation(elevation,tile);
	face->release();
}

bool PlanetElevationRequest::isRelevant(int id)
{
	return true;
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

// XXX temp
extern CaveGenerator caves;

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

	caves.generate();

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

					const auto blockReponse=planet.planetInfo->planetGenerator->getCharacteristic(prod_id, pos);
					const int height = elevationToBlockHeight(blockReponse.elevation);
					const blockTypes::T tile = blockReponse.tile;

					//TEMP (pour tester)
					const int waterHeight=CHUNK_N*MINIWORLD_H/2.f;
					if(height<waterHeight)
					{
						//UNDER THE SEAAAAAA
						for(int cy=0;cy<h;cy++)
						{
							yPos=pyPos;
							const int vy=cy*CHUNK_N;
							for(int j=0;j<(CHUNK_N+2);j++)
							{
								if (vy+py+j <= height) data[yPos]=blockTypes::sand;
								else if (vy+py+j <= waterHeight) data[yPos]=blockTypes::water;
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
								if (vy+py+j == height) data[yPos]=tile;
								else if (vy+py+j < height) data[yPos] = blockTypes::dirt;
								else if (vy+py+j == height+1 && rand()%100 == 1) data[yPos]=blockTypes::flower_red;
								else data[yPos]=blockTypes::air;

								yPos+=(CHUNK_N+2);
							}
							pyPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w;
						}
					}
					// cave
					auto holes = caves.getHolesList(px+i+cx*(CHUNK_N),pz+k+cz*(CHUNK_N));
					for(auto it = holes.begin(); it!=holes.end();++it)
					{
						if (it->second < height-100) continue;
						if (it->first > height) break;
						for(int i=it->first;i<=it->second;++i)
						{
							int y = (i % CHUNK_N)+1;
							int cy = i / CHUNK_N;
							data[zPos+y*(CHUNK_N+2)+cy*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w] = blockTypes::air;
							
							if ( y == (CHUNK_N) and cy != (MINIWORLD_H-1))
							{
								data[zPos+(0)*(CHUNK_N+2)+(cy+1)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w] = blockTypes::air;
							}
							if ( y==1 and cy!=0)
							{
								data[zPos+(CHUNK_N+1)*(CHUNK_N+2)+(cy-1)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w] = blockTypes::air;
							}
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


bool WorldChunkRequest::isRelevant(int id)
{
	return not chunk->getPointer()->isConstructionCanceled();
	//return true;
}
void WorldChunkRequest::process(int id)
{
	generateWorldData(id, planet, (chunkVal*)data, 1, 1, 1, px, py, pz, origin, v1, v2);
	computeChunkFaces((chunkVal*)data, 1, 1, 1, 0, 0, 0, px, py, pz, vArray);
}

void WorldChunkRequest::update(void)
{
	if(!isCanceled)
	{
		chunk->getPointer()->updateData((chunkVal*)data, vArray);
	}
	chunk->release();
}

//MiniWorldDataRequest stuff
MiniWorldDataRequest::MiniWorldDataRequest(Planet& p, MiniWorld& mw, glm::vec3 o, glm::vec3 v1, glm::vec3 v2, int x, int y, int z, ContentHandler& ch):
	px(x),
	py(y),
	pz(z),
	origin(o),
	v1(v1),
	v2(v2),
	planet(p),
	name(mw.getName()),
	contentHandler(ch)
{
	miniworld=mw.getTptr();
	miniworld->grab();
}

MiniWorldDataRequest::~MiniWorldDataRequest()
{}


bool MiniWorldDataRequest::isRelevant(int id)
{
	return not miniworld->getPointer()->isConstructionCanceled();
}

void MiniWorldDataRequest::process(int id)
{
	TrackerPointer<ChunkCacheEntry>* cce=contentHandler.cache.get(name);
	if(!cce)generateWorldData(id, planet, (chunkVal*)data, MINIWORLD_W, MINIWORLD_H, MINIWORLD_D, px, py, pz, origin, v1, v2);
	else{
		printf("LOADING FROM CACHE %s\n",name.c_str());
		memcpy(data,cce->getPointer()->getData(),sizeof(chunkVal)*MINIWORLD_W*MINIWORLD_H*MINIWORLD_D*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2));
		cce->release();
	}

	for(int i=0;i<MINIWORLD_W;i++)
		for(int j=0;j<MINIWORLD_H;j++)
			for(int k=0;k<MINIWORLD_D;k++)
				computeChunkFaces((chunkVal*)data, MINIWORLD_W, MINIWORLD_H, MINIWORLD_D, i, j, k, px+i*CHUNK_N, py+j*CHUNK_N, pz+k*CHUNK_N, vArray[i][j][k]);
}

void MiniWorldDataRequest::update(void)
{
	//if (not isCanceled)
	miniworld->getPointer()->updateChunks(data, vArray);
	miniworld->release();
}

//MiniWorldDeletionRequest stuff
MiniWorldDeletionRequest::MiniWorldDeletionRequest(MiniWorld& mw, ContentHandler& ch):
	contentHandler(ch)
{
	miniworld=mw.getTptr();
	miniworld->grab();
}

MiniWorldDeletionRequest::~MiniWorldDeletionRequest()
{}

bool MiniWorldDeletionRequest::isRelevant(int id)
{
	return true;
}

void MiniWorldDeletionRequest::process(int id)
{
	if(miniworld->getNumRef()>1)return;
	contentHandler.cache.save(miniworld->getPointer());
	miniworld->release();
}

void MiniWorldDeletionRequest::update(void)
{

}

//SolarSystemDataRequest stuff
SolarSystemDataRequest::SolarSystemDataRequest(SolarSystem& ss, ContentHandler& ch):
	contentHandler(ch)
{
	solarSystem=ss.getTptr();
	solarSystem->grab();
}

SolarSystemDataRequest::~SolarSystemDataRequest()
{}

bool SolarSystemDataRequest::isRelevant(int id)
{
	return true;
}

//idée ici c'est de générer les planetInfo côté producer (ie process) puis de faire l'initialisation des objets côté consumer (ie update)
void SolarSystemDataRequest::process(int id)
{
	//TEMP
	numPlanets=1;
	planets=new Planet*[numPlanets];
}

#include <sstream>


// TODO TODO TODO TODO TODO TODO
// TODO                     TODO
// TODO   Faire des delete  TODO
// TODO                     TODO
// TODO TODO TODO TODO TODO TODO

void SolarSystemDataRequest::update(void)
{
	for(int i=0;i<numPlanets;i++)
	{
		PlanetInfo pitest(
		);
		std::ostringstream oss;
		oss << i;
		planets[i]=
			new Planet(
				new PlanetInfo(
					new EllipticalTrajectory(
						glm::vec3(0.0f),
						glm::mat3(10.0f*(i+1)),
						i*1.037f,
						100.0f*(i+1)
					),
					new PlanetGeneratorEarth(contentHandler.getMaxProducers())
				),
				contentHandler,
				oss.str()
		);
	}
	sun=new Sun(glm::vec3(0.0f));

	solarSystem->getPointer()->numPlanets=numPlanets;
	solarSystem->getPointer()->planets=planets;
	solarSystem->getPointer()->sun=sun;
	solarSystem->getPointer()->generated=true;
	solarSystem->release();
}

