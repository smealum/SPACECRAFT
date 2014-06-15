#include "data/ContentRequest.h"
#include "data/ContentHandler.h"
#include "world/BlockType.h"
#include "world/blockProcessing.h"
#include "MiniWorld.h"
#include "utils/positionMath.h"
#include "utils/dbg.h"
#include "glm/gtc/noise.hpp"

#include "planetGenerator/PlanetGeneratorEarth.h"

#include "noise/Tree.h"

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
{
	face->release();
}

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
}

bool PlanetElevationRequest::isRelevant(int id)
{
	return true;
}

                                //if (vy+py+j > height &&
                                        //vy+py+j < height + tree.getHeight() + 1 &&
                                        //i >= CHUNK_N/2 - tree.getSize()/2 &&
                                        //i < CHUNK_N/2 + tree.getSize()/2 &&
                                        //k >= CHUNK_N/2 - tree.getSize()/2 &&
                                        //k < CHUNK_N/2 + tree.getSize()/2)
                                //{
                                    //int x = i - (CHUNK_N/2 - tree.getSize()/2),
                                        //y = vy+py+j - height -1,
                                        //z = k - (CHUNK_N/2 - tree.getSize()/2);
                                    //if (x >= 0 && x < tree.getSize() &&
                                            //y >=0 && y < tree.getHeight() &&
                                            //z >= 0 && z < tree.getSize())
                                    //{
                                        ////debug("Tree %d, %d, %d", x, y, z);
                                        //data[yPos] = tree.getBlock(x, y, z);
                                    //}
                                    //else
                                        //log_err("WTF OMG NOOOB");
                                //}

//MiniWorldDataRequest stuff
MiniWorldDataRequest::MiniWorldDataRequest(Planet& p, MiniWorld& mw, glm::vec3 o, glm::vec3 v1, glm::vec3 v2, int x, int y, int z, int numBlocks, ContentHandler& ch):
	px(x),
	py(y),
	pz(z),
	numBlocks(numBlocks),
	origin(o),
	v1(v1),
	v2(v2),
	planet(p),
	name(mw.getName()),
	modified(false),
	contentHandler(ch)
{
	miniworld=mw.getTptr();
	miniworld->grab();
}

MiniWorldDataRequest::~MiniWorldDataRequest()
{
	miniworld->release();
}

bool MiniWorldDataRequest::isRelevant(int id)
{
	return !miniworld->getPointer()->isConstructionCanceled();
}

void MiniWorldDataRequest::process(int id)
{
	TrackerPointer<ChunkCacheEntry>* cce=contentHandler.cache.get(name);
	if(!cce)planet.planetInfo->planetGenerator->generateWorldData(id,(chunkVal*)data,MINIWORLD_W,MINIWORLD_H,MINIWORLD_D,px,py,pz,origin,v1,v2);
	else{
		printf("LOADING FROM CACHE %s\n",name.c_str());
		memcpy(data,cce->getPointer()->getData(),sizeof(chunkVal)*MINIWORLD_W*MINIWORLD_H*MINIWORLD_D*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2));
		modified=cce->getPointer()->shouldBeSaved();
		cce->release();
	}

	for(int i=0;i<MINIWORLD_W;i++)
		for(int j=0;j<MINIWORLD_H;j++)
			for(int k=0;k<MINIWORLD_D;k++)
				computeChunkFaces((chunkVal*)data, MINIWORLD_W, MINIWORLD_H, MINIWORLD_D, i, j, k, px+i*CHUNK_N, py+j*CHUNK_N, pz+k*CHUNK_N, origin, v1, v2, numBlocks, vArray[i][j][k]);
}

void MiniWorldDataRequest::update(void)
{
	//if (not isCanceled)
	miniworld->getPointer()->updateChunks(data, vArray, modified);
}

//MiniWorldDeletionRequest stuff
MiniWorldDeletionRequest::MiniWorldDeletionRequest(MiniWorld& mw, ContentHandler& ch):
	contentHandler(ch)
{
	miniworld=mw.getTptr();
	miniworld->grab();
}

MiniWorldDeletionRequest::~MiniWorldDeletionRequest()
{
	miniworld->release();
}

bool MiniWorldDeletionRequest::isRelevant(int id)
{
	return true;
}

void MiniWorldDeletionRequest::process(int id)
{
	if(miniworld->getNumRef()>1)return;
	contentHandler.cache.save(miniworld->getPointer());
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
{
	solarSystem->release();
}

bool SolarSystemDataRequest::isRelevant(int id)
{
	return true;
}

#include "solarsystem/SolarSystemGeneratorSol.h"

//idée ici c'est de générer les planetInfo côté producer (ie process) puis de faire l'initialisation des objets côté consumer (ie update)
void SolarSystemDataRequest::process(int id)
{
	SolarSystemGeneratorSol ssgs(0, contentHandler);
	ssgs.generatePlanetInfos(planetInfos);
	planets=new Planet*[planetInfos.size()];
}

#include <sstream>


// TODO TODO TODO TODO TODO TODO
// TODO                     TODO
// TODO   Faire des delete  TODO
// TODO                     TODO
// TODO TODO TODO TODO TODO TODO

void SolarSystemDataRequest::update(void)
{
	int i=0;
	for(auto it=planetInfos.begin(); it!=planetInfos.end(); ++it)
	{
		std::ostringstream oss;
		oss << i;
		planets[i]=new Planet(*it, contentHandler, oss.str());
		i++;
	}
	sun=new Sun(glm::vec3(0.0f), 108.0f);
	// sun=new Sun(glm::vec3(0.0f), 40.0f);

	solarSystem->getPointer()->numPlanets=planetInfos.size();
	solarSystem->getPointer()->planets=planets;
	solarSystem->getPointer()->sun=sun;
	solarSystem->getPointer()->generated=true;
}

