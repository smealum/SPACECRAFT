#include "PlanetGeneratorEarth.h"
#include "PlanetInfo.h" 
#include "MiniWorld.h"
#include "utils/dbg.h"
#include "utils/positionMath.h"
#include "noise/CaveGenerator.h" // XXX debug
#include "noise/Tree.h"
#include <list>
#include "Application.h" // TODO remove include

// XXX temp
extern CaveGenerator caves;
using namespace std;
using namespace glm;

PlanetGeneratorEarth::PlanetGeneratorEarth(int nbThread):
	PlanetGenerator(nbThread),
	treePool(20)
{
	for (unsigned int i = 0; i < treePool.size(); i++)
	{
		treePool[i].generate(i);
		treePool[i].generateList();
	}
}

void PlanetGeneratorEarth::initGenerators() {
  for (int i = 0; i < nbThread; ++i) {
    generators.push_back(
        new PlanetNoiseGenerator(*((PlanetInfoEarth*)planetInfo)));
  }
}

PlanetGeneratorEarth::~PlanetGeneratorEarth() {}

void PlanetGeneratorEarth::setPlanetInfo(PlanetInfo* p)
{
	planetInfo = p;
	initGenerators();
}

void PlanetGeneratorEarth::generateWorldData(int threadId,
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

	// liste des positions d'arbres
	list<i32vec4> treePositions;

	//double t0 = glfwGetTime();
	
	// génération de la map des hauteurs
	const int offset = 6;
	const int heightMapX = MINIWORLD_W*CHUNK_N+2*offset;
	const int heightMapZ = MINIWORLD_D*CHUNK_N+2*offset;
	int          heightMap[heightMapX+1][heightMapZ+1];
	blockTypes::T  tileMap[heightMapX+1][heightMapZ+1];

	
	for(int x=0;x<heightMapX+1;x+=2)
	{
		for(int z=0;z<heightMapZ+1;z+=2)
		{
			// calcul des caractéristiques.
			const glm::vec3 pos=origin+((v1*float(px+x-offset))+(v2*float(pz+z-offset)))/float(planetInfo->numBlocks);
			const auto blockReponse=getCharacteristic(threadId, pos);
			heightMap[x][z] = elevationToBlockHeight(blockReponse.elevation, planetInfo->numBlocks) ;
			tileMap[x][z]   = blockReponse.tile;
		}
	}

	// interpolation des caractéristiques
	for(int x=0;x<heightMapX;x++)
	{
		for(int z=0;z<heightMapZ;z++)
		{
			int cx = int(x/2)*2;
			int cz = int(z/2)*2;
			int cxx = int((x+2)/2)*2;
			int czz = int((z+2)/2)*2;

			double xx = double(x-cx)*0.5;
			double zz = double(z-cz)*0.5;
			
			heightMap[x][z] = 
				heightMap[cx][cz] * (1.0-xx)*(1.0-zz) +
				heightMap[cx+2][cz] * (xx)*(1.0-zz) +
				heightMap[cx][cz+2] * (1.0-xx)*(zz) +
				heightMap[cx+2][cz+2]* (xx)*(zz);
			tileMap[x][z] = tileMap[cxx][czz];
		}
	}
	

	//for(int x=0;x<heightMapX;++x)
	//{
		//for(int z=0;z<heightMapZ;++z)
		//{
			//// calcul des caractéristiques.
			//const glm::vec3 pos=origin+((v1*float(px+x-offset))+(v2*float(pz+z-offset)))/float(planetInfo->numBlocks);
			//const auto blockReponse=getCharacteristic(threadId, pos);
			//heightMap[x][z] = elevationToBlockHeight(blockReponse.elevation, planetInfo->numBlocks) ;
			//tileMap[x][z]   = blockReponse.tile;
		//}
	//}


	//double t1 = glfwGetTime();

	// génération des blocs
	for(int cx=0;cx<w;cx++)
	{
		pzPos=pxPos;
		//const int vx=cx*(CHUNK_N+2);
		for(int cz=0;cz<d;cz++)
		{
			xPos=pzPos;
			//const int vz=cz*(CHUNK_N+2);
			for(int i=0;i<(CHUNK_N+2);i++)
			{
				zPos=xPos;
				for(int k=0;k<(CHUNK_N+2);k++)
				{
					pyPos=zPos;
					
					const auto height=heightMap[i+(CHUNK_N)*cx+offset][k+(CHUNK_N)*cz+offset];
					const auto tile=tileMap[i+(CHUNK_N)*cx+offset][k+(CHUNK_N)*cz+offset];

					//TEMP (pour tester)
					const int waterHeight=planetInfo->waterLevelBlock;
					int caveHeightMax;
					int caveHeightMin;
					caveHeightMax = height;
					caveHeightMax -= glm::clamp(waterHeight-height+3,0,4);
					
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
					caveHeightMin = caveHeightMax - 100;
					auto holes = caves.getHolesList(px+i+cx*(CHUNK_N),pz+k+cz*(CHUNK_N));
					for(auto it = holes.begin(); it!=holes.end();++it)
					{
						int a = std::max(caveHeightMin,it->first);
						int b = std::min(caveHeightMax,it->second);

						if (it->first  > caveHeightMax) break;

						for(int i=a;i<=b;++i)
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

	//double t2 = glfwGetTime();

	// ajout des arbres 
	for(int x=0;x<heightMapX;++x)
	{
		for(int z=0;z<heightMapZ;++z)
		{
			// ajout des arbres.
			int rx = (px + x - offset);
			int rz = (pz + z - offset);
			unsigned long long randomSource = rx*61311+rz*231331;
			//(randomSource) ^= (randomSource) >> 23;               
			//(randomSource) *= 0x2127599bf4325c37ULL;   
			//(randomSource) ^= (randomSource) >> 47;
			randomSource ^= (randomSource >> 29) & 0x5555555555555555ULL;
			randomSource ^= (randomSource << 17) & 0x71D67FFFEDA60000ULL;
			randomSource ^= rz;
			randomSource ^= (randomSource << 37) & 0xFFF7EEE000000000ULL;
			randomSource ^= (randomSource >> 43);

			// ajout des arbres
			if (
					(randomSource % 200 == 100) and
					tileMap[x][z] == blockTypes::grass
			)
			treePositions.push_back(glm::i32vec4(x-offset,heightMap[x][z], z-offset ,randomSource));
		}
	}

	// list positions ou poser des arbres
	for(auto tPos = treePositions.begin(); tPos != treePositions.end(); ++tPos)
	{
		Tree& t = treePool[tPos->w % treePool.size()];
		auto& tList = t.generateList();

		for( auto it = tList.begin(); it!=tList.end(); ++it)
		{
			const glm::i32vec3& v = it->first;

			putBlock(data,
					tPos->x+v.x,
					tPos->y+v.y-2,
					tPos->z+v.z,
					it->second);
		}
	}
	//double t3 = glfwGetTime();

	//log_info("%f %f %f",t1-t0,t2-t1,t3-t2);
	
	//for(int x=0;x<16;++x)
	//for(int y=0;y<16;++y)
	//for(int z=0;z<100;++z)
	//{
		//putBlock(data,x,y,z,blockTypes::snow);
	//}
	
}

inline double max(double x, double y)
{
	return (x>y)? x : y;
}

inline float min(float a, float b)
{
	return (a<b)?a:b;
}

PlanetGeneratorResponse PlanetGeneratorEarth::getCharacteristic(int threadId, const glm::vec3& pos)
{
	glm::vec3 posn = glm::normalize(pos);
	float temperature = getTemperature(posn);
	float humidity = getHumidity(posn);
	float elevation = getElevation(threadId,posn);
	double block_height=delevationToBlockHeight(elevation, planetInfo->numBlocks);

	if (block_height>planetInfo->waterLevelBlock) //  terre
	{
		// e > 0
		float e = (elevation - 1.001) * 1000.f ;

		float sandCoef  = 3.0*temperature + 0.4*e - humidity;
		float snowCoef  = -2.0*temperature+ 1.4*e + 0.3*humidity;
		float stoneCoef = snowCoef+0.01;
		float grassCoef = 0.5 + 0.5*abs(humidity);

		// inihibition
		// (pas de sable près de l'eau)
		//if (e<0.05) sandCoef = 0.0;
		// (A partir d'un moment la neige recouvre les cailloux
		stoneCoef = min(stoneCoef,1.0);


	
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
			default : return {1.0,blockTypes::grass};
		}

	}else{
		// mer
		return {elevation,blockTypes::sand};
	}
}

float PlanetGeneratorEarth::getElevation(int threadId, const glm::vec3 &coord)
{
  float elevation =
      (generators[threadId]->getElevation(glm::normalize(coord)) + 1.0) / 2.0f;
  return blockHeightToElevation(elevation * float(CHUNK_N * MINIWORLD_H),
                                planetInfo->numBlocks);
}

float PlanetGeneratorEarth::getTemperature(const glm::vec3& pos)
{
	float distanceToEquatorFactor = 1.0-2.0*fabs(
			glm::dot(
				glm::normalize(pos),
				glm::normalize(planetInfo->axis)
		 ));
	float noise1 = glm::simplex(pos*10.f) * 0.1;
	float noise2 = glm::simplex(pos*100.f) * 0.01;

	return glm::clamp(distanceToEquatorFactor+noise1+noise2,-1.f,1.f);
}

float PlanetGeneratorEarth::getHumidity(const glm::vec3& pos)
{
	float noise = glm::simplex(pos*2.f) *  1.4;
	
	return glm::clamp(noise,-1.f,1.f);
}
