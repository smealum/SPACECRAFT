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

	int height=MINIWORLD_H*CHUNK_N/2;

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
	return {planetInfo->waterLevelElevation,blockTypes::moondust};
}

float PlanetGeneratorMoon::getElevation(int threadId, const glm::vec3 &coord)
{
	return planetInfo->waterLevelElevation;
}
