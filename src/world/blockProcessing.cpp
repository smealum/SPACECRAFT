#include "blockProcessing.h"
#include "utils/positionMath.h"

//TODO pour computeChunkFaces et generateWorldData : NETTOYER (on peut clairement faire plus jolie et lisible)

// DOCUMENTATION:
//
// Valeurs des bornes:
// 0<=px<w
// 0<=py<h
// 0<=pz<d
// -1<=i<CHUNK_N+1
// -1<=j<CHUNK_N+1
// -1<=k<CHUNK_N+1

using namespace glm;

const glm::vec3 o[]={glm::vec3(0.0,0.0,0.0), //bottom
						glm::vec3(0.0,1.0,0.0), //top
						glm::vec3(0.0,0.0,0.0), //left
						glm::vec3(1.0,0.0,1.0), //right
						glm::vec3(1.0,0.0,0.0), //near
						glm::vec3(0.0,0.0,1.0), //far
						glm::vec3(0.0,0.0,0.0), //diag11
						glm::vec3(1.0,0.0,1.0), //diag12
						glm::vec3(1.0,0.0,0.0), //diag21
						glm::vec3(0.0,0.0,1.0) //diag22
						};

const glm::vec3 d1[]={glm::vec3(1.0,0.0,0.0), //bottom
						glm::vec3(0.0,0.0,1.0), //top
						glm::vec3(0.0,0.0,1.0), //left
						glm::vec3(0.0,0.0,-1.0), //right
						glm::vec3(-1.0,0.0,0.0), //near
						glm::vec3(1.0,0.0,0.0), //far
						glm::vec3(1.0,0.0,1.0), //diag11
						glm::vec3(-1.0,0.0,-1.0), //diag12
						glm::vec3(-1.0,0.0,1.0), //diag21
						glm::vec3(1.0,0.0,-1.0) //diag22
						};

const glm::vec3 d2[]={glm::vec3(0.0,0.0,1.0), //bottom
						glm::vec3(1.0,0.0,0.0), //top
						glm::vec3(0.0,1.0,0.0), //left
						glm::vec3(0.0,1.0,0.0), //right
						glm::vec3(0.0,1.0,0.0), //near
						glm::vec3(0.0,1.0,0.0), //far
						glm::vec3(0.0,1.0,0.0), //diag11
						glm::vec3(0.0,1.0,0.0), //diag12
						glm::vec3(0.0,1.0,0.0), //diag21
						glm::vec3(0.0,1.0,0.0) //diag22
						};

const glm::vec3 n[]={glm::vec3(0.0,-1.0,0.0), //bottom
						glm::vec3(0.0,1.0,0.0), //top
						glm::vec3(-1.0,0.0,0.0), //left
						glm::vec3(1.0,0.0,1.0), //right
						glm::vec3(0.0,0.0,-1.0), //near
						glm::vec3(0.0,0.0,1.0), //far
						glm::normalize(glm::vec3(1.0,0.0,-1.0)), //diag11
						glm::normalize(glm::vec3(-1.0,0.0,1.0)), //diag12
						glm::normalize(glm::vec3(1.0,0.0,1.0)), //diag21
						glm::normalize(glm::vec3(-1.0,0.0,-1.0)) //diag22
						};

void generateFace(std::vector<GL_Vertex>& vArray, glm::vec3 pos, int facedir, glm::vec3 origin, glm::vec3 v1, glm::vec3 v2, int numBlocks, int tile)
{
	GL_Vertex v;

	glm::vec3 pos1, pos2;
	float y;

	pos1=pos+o[facedir];
	pos2=v1*pos1.x+v2*pos1.z;
	y=blockHeightToElevation(pos1.y,numBlocks);

	const glm::vec3 rn=(normalize(origin+(pos2)/float(numBlocks))*y);

	v.position=(normalize(origin+(pos2)/float(numBlocks))*y);
	v.texcoord=glm::vec2(1,1);
	v.tile=tile;
	v.normal=glm::normalize(n[facedir].x*v1+n[facedir].z*v2+n[facedir].y*rn);
	vArray.push_back(v);


	pos1=pos+o[facedir]+d1[facedir];
	pos2=v1*pos1.x+v2*pos1.z;
	y=1.0+pos1.y/float(numBlocks);

	v.position=(normalize(origin+(pos2)/float(numBlocks))*y);
	v.texcoord=glm::vec2(0,1);
	v.tile=tile;
	v.normal=glm::normalize(n[facedir].x*v1+n[facedir].z*v2+n[facedir].y*rn);
	vArray.push_back(v);


	pos1=pos+o[facedir]+d2[facedir];
	pos2=v1*pos1.x+v2*pos1.z;
	y=1.0+pos1.y/float(numBlocks);

	v.position=(normalize(origin+(pos2)/float(numBlocks))*y);
	v.texcoord=glm::vec2(1,0);
	v.tile=tile;
	v.normal=glm::normalize(n[facedir].x*v1+n[facedir].z*v2+n[facedir].y*rn);
	vArray.push_back(v);



	pos1=pos+o[facedir]+d1[facedir]+d2[facedir];
	pos2=v1*pos1.x+v2*pos1.z;
	y=1.0+pos1.y/float(numBlocks);

	v.position=(normalize(origin+(pos2)/float(numBlocks))*y);
	v.texcoord=glm::vec2(0,0);
	v.tile=tile;
	v.normal=glm::normalize(n[facedir].x*v1+n[facedir].z*v2+n[facedir].y*rn);
	vArray.push_back(v);


	pos1=pos+o[facedir]+d2[facedir];
	pos2=v1*pos1.x+v2*pos1.z;
	y=1.0+pos1.y/float(numBlocks);

	v.position=(normalize(origin+(pos2)/float(numBlocks))*y);
	v.texcoord=glm::vec2(1,0);
	v.tile=tile;
	v.normal=glm::normalize(n[facedir].x*v1+n[facedir].z*v2+n[facedir].y*rn);
	vArray.push_back(v);


	pos1=pos+o[facedir]+d1[facedir];
	pos2=v1*pos1.x+v2*pos1.z;
	y=1.0+pos1.y/float(numBlocks);

	v.position=(normalize(origin+(pos2)/float(numBlocks))*y);
	v.texcoord=glm::vec2(0,1);
	v.tile=tile;
	v.normal=glm::normalize(n[facedir].x*v1+n[facedir].z*v2+n[facedir].y*rn);
	vArray.push_back(v);
}

//TODO : optimiser pour éviter les multiplications à chaque fois
//(juste utiliser un pointeur à chaque fois...)
void computeChunkFaces(chunkVal* data,
		int w, int h, int d, //array sizes (in chunks)
		int sx, int sy, int sz, //chunk in array (in chunks)
		int px, int py, int pz, //chunk offset in world (in blocks)
		glm::vec3 origin, glm::vec3 v1, glm::vec3 v2,
		int numBlocks,
		std::vector<GL_Vertex>& vArray) //output
{
	vArray.clear();
	auto &blockType = BlockType::getInstance();

	chunkVal previous,current;
	// X
	for(int y=0;y<CHUNK_N;++y)
	for(int z=0;z<CHUNK_N;++z)
	{
		previous = accessArray(data,w,h,d,sx,sy,sz,-1,y,z);
		for(int x=0;x<CHUNK_N+1;++x)
		{
			current = accessArray(data,w,h,d,sx,sy,sz,x,y,z);
			if (blockShouldBeFace(current,previous)) {
				generateFace(vArray, vec3(px+x,py+y,pz+z), 2, origin, v1, v2, numBlocks, getBlockID(current,blockPlane::side));
			}else if (blockShouldBeFace(previous,current)) {
				generateFace(vArray, vec3(px+x-1,py+y,pz+z), 3, origin, v1, v2, numBlocks, getBlockID(previous,blockPlane::side));
			}
			previous=current;
		}
	}

	// Y
	for(int x=0;x<CHUNK_N;++x)
	for(int z=0;z<CHUNK_N;++z)
	{
		previous = accessArray(data,w,h,d,sx,sy,sz,x,-1,z);
		for(int y=0;y<CHUNK_N+1;++y)
		{
			current = accessArray(data,w,h,d,sx,sy,sz,x,y,z);
			if (blockShouldBeFace(current,previous)) {
				generateFace(vArray, vec3(px+x,py+y,pz+z), 0, origin, v1, v2, numBlocks, getBlockID(current,blockPlane::bottom));
			} else if (blockShouldBeFace(previous,current)) {
				generateFace(vArray, vec3(px+x,py+y-1,pz+z), 1, origin, v1, v2, numBlocks, getBlockID(previous,blockPlane::top));
			}
			previous=current;
		}
	}

	// Z
	for(int x=0;x<CHUNK_N;++x)
	for(int y=0;y<CHUNK_N;++y)
	{
		previous = accessArray(data,w,h,d,sx,sy,sz,x,y,-1);
		for(int z=0;z<CHUNK_N+1;++z)
		{
			current = accessArray(data,w,h,d,sx,sy,sz,x,y,z);
			if (blockShouldBeFace(current,previous)) {
				generateFace(vArray, vec3(px+x,py+y,pz+z), 4, origin, v1, v2, numBlocks, getBlockID(current,blockPlane::side));
			}else if (blockShouldBeFace(previous,current)) {
				generateFace(vArray, vec3(px+x,py+y,pz+z-1), 5, origin, v1, v2, numBlocks, getBlockID(previous,blockPlane::side));
			}
			previous=current;
		}
	}

	//sprites
	for(int x=0;x<CHUNK_N;++x)
	for(int y=0;y<CHUNK_N;++y)
	for(int z=0;z<CHUNK_N;++z)
	{
		current = accessArray(data,w,h,d,sx,sy,sz,x,y,z);
		if (blockStyleID[current] == blockStyle::sprite)
		{
			generateFace(vArray, vec3(px+x,py+y,pz+z), 6, origin, v1, v2, numBlocks, getBlockID(current,blockPlane::top));
			generateFace(vArray, vec3(px+x,py+y,pz+z), 7, origin, v1, v2, numBlocks, getBlockID(current,blockPlane::top));
			generateFace(vArray, vec3(px+x,py+y,pz+z), 8, origin, v1, v2, numBlocks, getBlockID(current,blockPlane::top));
			generateFace(vArray, vec3(px+x,py+y,pz+z), 9, origin, v1, v2, numBlocks, getBlockID(current,blockPlane::top));
		}
	}
}
