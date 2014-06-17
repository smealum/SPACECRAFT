#include "PlanetGeneratorMoon.h"
#include "PlanetInfo.h" 
#include "MiniWorld.h"
#include "utils/dbg.h"
#include "utils/positionMath.h"


PlanetGeneratorMoon::PlanetGeneratorMoon(int nbThread):
	PlanetGenerator(nbThread)
{
}

PlanetGeneratorMoon::~PlanetGeneratorMoon()
{
}

void PlanetGeneratorMoon::generateWorldData(int threadId,
			chunkVal* data,
			int w, int h, int d, // array sizes (in chunks)
			int px, int py, int pz, // offset in world
			glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) // toplevelCharacteristic
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

	// génération de la map des hauteurs
	const int offset=2;
	const int heightMapX = MINIWORLD_W*CHUNK_N+2*offset;
	const int heightMapZ = MINIWORLD_D*CHUNK_N+2*offset;
	int heightMap[heightMapX][heightMapZ];
	blockTypes::T tileMap[heightMapX][heightMapZ];

	for(int x=0;x<heightMapX;++x)
	{
		for(int z=0;z<heightMapZ;++z)
		{
			// calcul des caractéristiques.
			const glm::vec3 pos=origin+((v1*float(px+x-offset))+(v2*float(pz+z-offset)))/float(planetInfo->numBlocks);
			const auto blockReponse=getCharacteristic(threadId, pos);
			heightMap[x][z] = elevationToBlockHeight(blockReponse.elevation, planetInfo->numBlocks);
			tileMap[x][z] = blockReponse.tile;
		}
	}

	// génération des blocs
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
					const auto height=heightMap[i+(CHUNK_N)*cx+offset][k+(CHUNK_N)*cz+offset];

					for(int cy=0;cy<h;cy++)
					{
						yPos=pyPos;
						const int vy=cy*CHUNK_N;
						for(int j=0;j<(CHUNK_N+2);j++)
						{
							if(vy+py+j<height)data[yPos]=blockTypes::moondust;
							else data[yPos]=blockTypes::air;

							yPos+=(CHUNK_N+2);
						}
						pyPos+=(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*w;
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

PlanetGeneratorResponse PlanetGeneratorMoon::getCharacteristic(int threadId, const glm::vec3& pos)
{
	return {getElevation(threadId, glm::normalize(pos)), blockTypes::moondust};
}

float PlanetGeneratorMoon::getElevation(int threadId, const glm::vec3 &coord)
{
	float elev=(glm::simplex(coord*8.0f)+1.0f)/8+0.4f;
	float v=glm::simplex(coord*128.0f);
	if(v<0.0f)elev+=v/16;
	return blockHeightToElevation((elev)*MINIWORLD_H*CHUNK_N, planetInfo->numBlocks);
}
