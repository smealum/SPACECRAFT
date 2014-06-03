#include "data/ContentRequest.h"
#include "world/BlockType.h"
#include "MiniWorld.h"
#include "utils/dbg.h"

#include <cstdio>

using namespace glm;

//PlanetElevationRequest stuff
PlanetElevationRequest::PlanetElevationRequest(Planet& p, PlanetFace& pf, glm::vec3 c):
	planet(p),
	coord(c)
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
	// elevation=getElevation(glm::normalize(coord));
	elevation=1.0f+(getElevation(id, planet, glm::normalize(coord))*CHUNK_N*MINIWORLD_H)/PLANETFACE_BLOCKS; //faudra passer par le helper hypothétique à terme, au cas où on aurait envie de changer les dimensions des blocs...
}

void PlanetElevationRequest::update(void)
{
	face->getPointer()->updateElevation(elevation);
	face->release();
}

//WorldChunkRequest stuff
WorldChunkRequest::WorldChunkRequest(Planet& p, Chunk& c, float elevation, glm::vec3 o, glm::vec3 v1, glm::vec3 v2, int x, int y, int z):
	planet(p),
	origin(o),
	elevation(elevation),
	v1(v1),
	v2(v2),
	px(x),
	py(y),
	pz(z)
{
	chunk=c.getTptr();
	chunk->grab();
}

WorldChunkRequest::~WorldChunkRequest()
{}

#define accessArray(data, w, h, d, px, py, pz, i, j, k) (data)[((px)+(py)*(w)+(pz)*(w)*(h))*CHUNK_N*CHUNK_N*CHUNK_N+(i)+(j)*(CHUNK_N)+(k)*(CHUNK_N)*(CHUNK_N)]

//TODO : optimiser pour éviter les multiplications à chaque fois
//(juste utiliser un pointeur à chaque fois...)
void computeChunkFaces(char* data,
		int w, int h, int d, //array sizes (in chunks)
		int sx, int sy, int sz, //chunk in array (in chunks)
		int px, int py, int pz, //chunk offset in world (in blocks)
		std::vector<GL_Vertex>& vArray) //output
{
    vArray.clear();
    auto blockType = BlockType::getInstance();

	// X
	for(int y=0;y<CHUNK_N;++y)
	for(int z=0;z<CHUNK_N;++z)
	for(int x=1;x<CHUNK_N;++x)
	{
		if(accessArray(data,w,h,d,sx,sy,sz,x,y,z))
		{
			if (!accessArray(data,w,h,d,sx,sy,sz,x-1,y,z))
			{
				GL_Vertex v;
				v.facedir=2;
				v.texcoord= blockType.getTexcoord(
					(BlockType::T)int(accessArray(data,w,h,d,sx,sy,sz,x,y,z)-1),
					BlockType::side
					);
				v.position=vec3(px+x,py+y,pz+z);
				vArray.push_back(v);
			}
		}else{
			if (accessArray(data,w,h,d,sx,sy,sz,x-1,y,z))
			{
				GL_Vertex v;
				v.facedir=3;
				v.texcoord=blockType.getTexcoord(
					(BlockType::T)int(accessArray(data,w,h,d,sx,sy,sz,x-1,y,z)-1),
					BlockType::side
					);
				v.position=vec3(px+x-1,py+y,pz+z);
				vArray.push_back(v);
			}
		}
	}

	// Y
	for(int x=0;x<CHUNK_N;++x)
	for(int z=0;z<CHUNK_N;++z)
	for(int y=1;y<CHUNK_N;++y)
	{
		if(accessArray(data,w,h,d,sx,sy,sz,x,y,z))
		{
			if (!accessArray(data,w,h,d,sx,sy,sz,x,y-1,z))
			{
				GL_Vertex v;
				v.facedir=0;
				v.texcoord=blockType.getTexcoord(
					(BlockType::T)int(accessArray(data,w,h,d,sx,sy,sz,x,y,z)-1),
					BlockType::top
					);
				v.position=vec3(px+x,py+y,pz+z);
				vArray.push_back(v);
			}
		}else{
			if (accessArray(data,w,h,d,sx,sy,sz,x,y-1,z))
			{
				GL_Vertex v;
				v.facedir=1;
				v.texcoord=blockType.getTexcoord(
					(BlockType::T)int(accessArray(data,w,h,d,sx,sy,sz,x,y-1,z)-1),
					BlockType::top
					);
				v.position=vec3(px+x,py+y-1,pz+z);
				vArray.push_back(v);
			}
		}
	}

	// Z
	for(int x=0;x<CHUNK_N;++x)
	for(int y=0;y<CHUNK_N;++y)
	for(int z=1;z<CHUNK_N;++z)
	{
		if(accessArray(data,w,h,d,sx,sy,sz,x,y,z))
		{
			if (!accessArray(data,w,h,d,sx,sy,sz,x,y,z-1))
			{
				GL_Vertex v;
				v.facedir=4;
				v.texcoord=blockType.getTexcoord(
					(BlockType::T)int(accessArray(data,w,h,d,sx,sy,sz,x,y,z)-1),
					BlockType::side
					);
				v.position=vec3(px+x,py+y,pz+z);
				vArray.push_back(v);
			}
		}else{
			if (accessArray(data,w,h,d,sx,sy,sz,x,y,z-1))
			{
				GL_Vertex v;
				v.facedir=5;
				v.texcoord=blockType.getTexcoord(
					(BlockType::T)int(accessArray(data,w,h,d,sx,sy,sz,x,y,z-1)-1),
					BlockType::side
					);
				v.position=vec3(px+x,py+y,pz-1+z);
				vArray.push_back(v);
			}
		}
	}
}

//TODO : optimiser et proprifier
//(on peut largement optimiser les accès à data, éviter *énormément* de multiplications)
void generateWorldData(int prod_id, Planet& planet, char* data,
		int w, int h, int d, //array sizes (in chunks)
		int px, int py, int pz, //offset in world
		glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) //toplevel characteristics
{
	for(int cx=0;cx<w;cx++)
	{
		const int vx=cx*CHUNK_N;
		for(int cz=0;cz<d;cz++)
		{
			const int vz=cz*CHUNK_N;
			for(int i=0;i<CHUNK_N;i++)
			{
				for(int k=0;k<CHUNK_N;k++)
				{
					const glm::vec3 pos=origin+((v1*float(vx+px+i))+(v2*float(vz+pz+k)))/float(PLANETFACE_BLOCKS);
					const int height=int(getElevation(prod_id, planet, pos)*CHUNK_N*MINIWORLD_H);
					for(int cy=0;cy<h;cy++)
					{
						const int vy=cy*CHUNK_N;
						for(int j=0;j<CHUNK_N;j++)
						{
							if(vy+py+j==height)accessArray(data,w,h,d,cx,cy,cz,i,j,k)=1; // 1 -> 0 <=> grass
							else if(vy+py+j<height)accessArray(data,w,h,d,cx,cy,cz,i,j,k)=3; // 3 -> 2 <=> dirt
							else accessArray(data,w,h,d,cx,cy,cz,i,j,k)=0;
						}
					}
				}
			}
		}
	}
}

void WorldChunkRequest::process(int id)
{
	generateWorldData(id, planet, (char*)data, 1, 1, 1, px, py, pz, origin, v1, v2);
	computeChunkFaces((char*)data, 1, 1, 1, 0, 0, 0, px, py, pz, vArray);
}

void WorldChunkRequest::update(void)
{
	chunk->getPointer()->updateData((char*)data, vArray);
	chunk->release();
}

//MiniWorldDataRequest stuff
MiniWorldDataRequest::MiniWorldDataRequest(Planet& p, MiniWorld& mw, glm::vec3 o, glm::vec3 v1, glm::vec3 v2, int x, int y, int z):
	planet(p),
	origin(o),
	v1(v1),
	v2(v2),
	px(x),
	py(y),
	pz(z)
{
	miniworld=mw.getTptr();
	miniworld->grab();
}

MiniWorldDataRequest::~MiniWorldDataRequest()
{}

void MiniWorldDataRequest::process(int id)
{
	generateWorldData(id, planet, (char*)data, MINIWORLD_W, MINIWORLD_H, MINIWORLD_D, px, py, pz, origin, v1, v2);

	for(int i=0;i<MINIWORLD_W;i++)
		for(int j=0;j<MINIWORLD_H;j++)
			for(int k=0;k<MINIWORLD_D;k++)
				computeChunkFaces((char*)data, MINIWORLD_W, MINIWORLD_H, MINIWORLD_D, i, j, k, px+i*CHUNK_N, py+j*CHUNK_N, pz+k*CHUNK_N, vArray[i][j][k]);
}

void MiniWorldDataRequest::update(void)
{
	miniworld->getPointer()->updateChunks(data, vArray);
	miniworld->release();
}