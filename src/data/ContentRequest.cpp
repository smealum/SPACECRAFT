#include "data/ContentRequest.h"

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

void PlanetElevationRequest::process(void)
{
	// elevation=1.0+glm::simplex(glm::normalize(coord))*0.1f;
	elevation=1.0+fabs(glm::simplex(glm::normalize(coord*1000.0f)))*0.1f+fabs(glm::simplex(glm::normalize(coord*100.0f)))*0.01f+glm::simplex(glm::normalize(coord*10.0f))*0.001;
}

void PlanetElevationRequest::update(void)
{
	face->getPointer()->updateElevation(elevation);
	face->release();
}

//WorldChunkRequest stuff
WorldChunkRequest::WorldChunkRequest(Planet& p, Chunk& c, glm::vec3 o, glm::vec3 e, int x, int y, int z):
	planet(p),
	origin(o),
	end(e),
	px(x),
	py(y),
	pz(z)
{
	chunk=c.getTptr();
	chunk->grab();
}

WorldChunkRequest::~WorldChunkRequest()
{}

void WorldChunkRequest::computeChunk(void)
{
    vArray.clear();

	// X
	for(int y=0;y<CHUNK_N;++y)
	for(int z=0;z<CHUNK_N;++z)
	for(int x=1;x<CHUNK_N;++x)
	{
		if (data[x][y][z])
		{
			if (!data[x-1][y][z])
			{
				GL_Vertex v;
				v.color=vec4(
					double(x)/double(CHUNK_N),
					double(y)/double(CHUNK_N),
					double(z)/double(CHUNK_N),
					1.0);
				v.normal=vec3(-1,0,0);
				v.position=vec3(x,y,z)*BLOCK_SIZE;     vArray.push_back(v);
				v.position=vec3(x,y,z+1)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y+1,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y,z+1)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y+1,z+1)*BLOCK_SIZE; vArray.push_back(v);
				v.position=vec3(x,y+1,z)*BLOCK_SIZE;   vArray.push_back(v);
			}
		}else{
			if (data[x-1][y][z])
			{
				GL_Vertex v;
				v.color=vec4(
					double(x-1)/double(CHUNK_N),
					double(y)/double(CHUNK_N),
					double(z)/double(CHUNK_N),
					1.0);
				v.normal=vec3(1,0,0);
				v.position=vec3(x,y,z)*BLOCK_SIZE;     vArray.push_back(v);
				v.position=vec3(x,y+1,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y,z+1)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y,z+1)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y+1,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y+1,z+1)*BLOCK_SIZE; vArray.push_back(v);
			}
		}
	}

	// Y
	for(int x=0;x<CHUNK_N;++x)
	for(int z=0;z<CHUNK_N;++z)
	for(int y=1;y<CHUNK_N;++y)
	{
		if (data[x][y][z])
		{
			if (!data[x][y-1][z])
			{
				GL_Vertex v;
				v.color=vec4(
				double(x)/double(CHUNK_N),
				double(y)/double(CHUNK_N),
				double(z)/double(CHUNK_N),
				1.0
				);
				v.normal=vec3(0,-1,0);
				v.position=vec3(x,y,z)*BLOCK_SIZE;     vArray.push_back(v);
				v.position=vec3(x+1,y,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y,z+1)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x+1,y,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x+1,y,z+1)*BLOCK_SIZE; vArray.push_back(v);
				v.position=vec3(x,y,z+1)*BLOCK_SIZE;   vArray.push_back(v);
			}
		}else{
			if (data[x][y-1][z])
			{
				GL_Vertex v;
				v.color=vec4(
					double(x)/double(CHUNK_N),
					double(y-1)/double(CHUNK_N),
					double(z)/double(CHUNK_N),
					1.0);
				v.normal=vec3(0,1,0);
				v.position=vec3(x,y,z)*BLOCK_SIZE;     vArray.push_back(v);
				v.position=vec3(x,y,z+1)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x+1,y,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x+1,y,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y,z+1)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x+1,y,z+1)*BLOCK_SIZE; vArray.push_back(v);
			}
		}
	}

	// Z
	for(int x=0;x<CHUNK_N;++x)
	for(int y=0;y<CHUNK_N;++y)
	for(int z=1;z<CHUNK_N;++z)
	{
		if (data[x][y][z])
		{
			if (!data[x][y][z-1])
			{
				GL_Vertex v;
				v.color=vec4(
					double(x)/double(CHUNK_N),
					double(y)/double(CHUNK_N),
					double(z)/double(CHUNK_N),
					1.0);
				v.normal=vec3(0,0,-1);
				v.position=vec3(x,y,z)*BLOCK_SIZE;     vArray.push_back(v);
				v.position=vec3(x,y+1,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x+1,y,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y+1,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x+1,y+1,z)*BLOCK_SIZE; vArray.push_back(v);
				v.position=vec3(x+1,y,z)*BLOCK_SIZE;   vArray.push_back(v);
			}
		}else{
			if (data[x][y][z-1])
			{
				GL_Vertex v;
				v.color=vec4(
					double(x)/double(CHUNK_N),
					double(y)/double(CHUNK_N),
					double(z-1)/double(CHUNK_N),
					1.0);
				v.normal=vec3(0,0,1);
				v.position=vec3(x,y,z)*BLOCK_SIZE;     vArray.push_back(v);
				v.position=vec3(x+1,y,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y+1,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x,y+1,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x+1,y,z)*BLOCK_SIZE;   vArray.push_back(v);
				v.position=vec3(x+1,y+1,z)*BLOCK_SIZE; vArray.push_back(v);
			}
		}
	}
}

void WorldChunkRequest::process(void)
{
	//TEMP
	for(int i=0;i<CHUNK_N;i++)
	{
		for(int k=0;k<CHUNK_N;k++)
		{
			const int h=(int)((glm::simplex(glm::vec2(px*CHUNK_N+i,pz*CHUNK_N+k)*0.01f)+1.0f)*32);
			// const int h=CHUNK_N/2;
			// const int h=2;
			for(int j=0;j<CHUNK_N;j++)
			{
				if(py*CHUNK_N+j<=h)data[i][j][k]=1;
				else data[i][j][k]=0;
			}
		}
	}

	computeChunk();
}

void WorldChunkRequest::update(void)
{
	chunk->getPointer()->updateData(data, vArray);
	chunk->release();
}
