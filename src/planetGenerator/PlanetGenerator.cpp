#include "PlanetGenerator.h"
#include "PlanetInfo.h" 
#include "utils/dbg.h"
#include "utils/positionMath.h"

PlanetGenerator::PlanetGenerator(int n):
	nbThread(n)
{
}

void PlanetGenerator::setPlanetInfo(PlanetInfo* p)
{
	planetInfo = p;
}


void putBlockLevel4(chunkVal* data,
	int x, int y, int z, // position of the bloc
	int px, int py, int pz, // position du chunk
	blockTypes::T b
)
{
	if (
			px<0 or
			py<0 or
			pz<0 or
			px>=MINIWORLD_W or
			py>=MINIWORLD_H or
			pz>=MINIWORLD_D
		) return;

	data[
		(x+1) + (CHUNK_N+2)*( 
		(y+1) + (CHUNK_N+2)*( 
		(z+1) + (CHUNK_N+2)*( 
		px    + (MINIWORLD_W)*(
		py    + (MINIWORLD_H)*(
		pz)))))
	] = b;
	//log_info("je write : %u %d %d %d %d %d %d", b, x, y, z, px, py, pz);
}

void putBlockLevel3(chunkVal* data,
	int x, int y, int z, // position of the bloc
	int px, int py, int pz, // position du chunk
	blockTypes::T b
)
{
	putBlockLevel4(data,x,y,z,px,py,pz,b);
	if (z==0)
	{
		putBlockLevel4(data,x,y,CHUNK_N,px,py,pz-1,b);
	}
	else if (y==CHUNK_N-1)
	{
		putBlockLevel4(data,x,y,-1,px,py,pz+1,b);
	}
}


void putBlockLevel2(chunkVal* data,
	int x, int y, int z, // position of the bloc
	int px, int py, int pz, // position du chunk
	blockTypes::T b
)
{
	putBlockLevel3(data,x,y,z,px,py,pz,b);
	if (y==0)
	{
		putBlockLevel3(data,x,CHUNK_N,z,px,py-1,pz,b);
	}
	else if (y==CHUNK_N-1)
	{
		putBlockLevel3(data,x,-1,z,px,py+1,pz,b);
	}
}

void putBlockLevel1(chunkVal* data,
	int x, int y, int z, // position of the bloc
	int px, int py, int pz, // position du chunk
	blockTypes::T b
)
{
	putBlockLevel2(data,x,y,z,px,py,pz,b);
	if (x==0)
	{
		putBlockLevel2(data,CHUNK_N,y,z,px-1,py,pz,b);
	}
	else if (x==CHUNK_N-1)
	{
		putBlockLevel2(data,-1,y,z,px+1,py,pz,b);
	}
}


void PlanetGenerator::putBlock(chunkVal* data,
	int x, int y, int z, // position of the bloc dans le miniworld
	blockTypes::T b
)
{
	if (
			x<0 or
			y<0 or
			z<0 or
			x>=MINIWORLD_W*CHUNK_N or
			y>=MINIWORLD_H*CHUNK_N or
			z>=MINIWORLD_D*CHUNK_N
	) return;
	int px = x / (CHUNK_N);
	int py = y / (CHUNK_N);
	int pz = z / (CHUNK_N);
	x -= px * CHUNK_N;
	y -= py * CHUNK_N;
	z -= pz * CHUNK_N;
	putBlockLevel1(data,x,y,z,px,py,pz,b);
}
