#include "PlanetGeneratorEarth.h"
#include "PlanetInfo.h" 
#include "MiniWorld.h"
#include "utils/dbg.h"
#include "utils/positionMath.h"
#include "noise/CaveGenerator.h" // XXX debug

// XXX temp
extern CaveGenerator caves;

PlanetGeneratorEarth::PlanetGeneratorEarth(int nbThread):
	PlanetGenerator(nbThread)
{
}

void PlanetGeneratorEarth::initGenerators()
{
	for(int i=0;i<nbThread;++i)
		generators.push_back(new PlanetNoiseGenerator(*planetInfo));
}

PlanetGeneratorEarth::~PlanetGeneratorEarth()
{

}


void PlanetGeneratorEarth::generateWorldData(int threadId,
			chunkVal* data,
			int w, int h, int d, // array sizes (in chunks)
			int px, int py, int pz, // offset in world
			glm::vec3 origin, glm::vec3 v1, glm::vec3 v2) const // toplevelCharacteristic
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

					const auto blockReponse=getCharacteristic(threadId, pos);
					const int height = elevationToBlockHeight(blockReponse.elevation);
					const blockTypes::T tile = blockReponse.tile;

					//TEMP (pour tester)
					const int waterHeight=CHUNK_N*MINIWORLD_H/2.f;
					int caveHeightMin;
					int caveHeightMax;
					if(height<waterHeight)
					{
						caveHeightMax = height-25;
						caveHeightMin = caveHeightMax-100;
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
						caveHeightMax = height+50;
						caveHeightMin = caveHeightMax-100;
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
						if (it->first  < caveHeightMin) continue;
						if (it->second > caveHeightMax) break;

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

inline double max(double x, double y)
{
	return (x>y)? x : y;
}

PlanetGeneratorResponse PlanetGeneratorEarth::getCharacteristic(int threadId, const glm::vec3& pos) const
{
	glm::vec3 posn = glm::normalize(pos);
	float temperature = getTemperature(posn);
	// log_info("temperature = %f",temperature);
	float humidity = getHumidity(posn);
	float elevation = getElevation(threadId,posn);
	double block_height=delevationToBlockHeight(elevation);

	if (block_height>double(CHUNK_N*MINIWORLD_H)*0.5) //  terre
	{
		// e > 0
		float e = (elevation - 1.001) * 1000.f ;

		float sandCoef = 4.0*temperature + 1.4*e - humidity;
		float snowCoef = -2.0*temperature+ 1.4*e + 0.3*humidity;
		float stoneCoef = snowCoef+0.01 - 0.1*humidity;
		float grassCoef = 0.05 + 2.0*abs(humidity);

		// inihibition
		// (pas de sable près de l'eau)
		//if (e<0.05) sandCoef = 0.0;
		// (A partir d'un moment la neige recouvre les cailloux
		stoneCoef = max(stoneCoef,0.8);

	
		// on choisit le plus grand
		int imax=0;int vmax=sandCoef;
		if (vmax<snowCoef) {vmax=snowCoef;imax=1;}
		if (vmax<stoneCoef) {vmax=stoneCoef;imax=2;}
		if (vmax<grassCoef) {vmax=grassCoef;imax=3;}
		
		
		switch(imax)
		{
			case 0: return {elevation,blockTypes::sand};
			case 1: return {elevation,blockTypes::snow};
			case 2: return {elevation,blockTypes::stone};
			case 3: return {elevation,blockTypes::grass};
		}

	}
	else // mer
	{
		return {elevation,blockTypes::water};
	}
}

float PlanetGeneratorEarth::getTemperature(const glm::vec3& pos) const
{
	float distanceToEquatorFactor = 1.0-2.0*fabs(
			glm::dot(
				glm::normalize(pos),
				glm::normalize(planetInfo->axis)
		 ));
	float noise1 = glm::simplex(pos*10.f) * 0.0;
	float noise2 = glm::simplex(pos*100.f) * 0.00;

	return glm::clamp(distanceToEquatorFactor+noise1+noise2,-1.f,1.f);
}

float PlanetGeneratorEarth::getHumidity(const glm::vec3& pos) const
{
	float noise = glm::simplex(pos*2.f) *  1.4;
	
	return glm::clamp(noise,-1.f,1.f);
}
