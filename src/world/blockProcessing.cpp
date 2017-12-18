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

namespace Face {

enum Id {
  Bottom,
  Top,
  Left,
  Right,
  Near,
  Far,
  Diag11,
  Diag12,
  Diag21,
  Diag22,
};

const glm::vec3 model[10][4] = {
  // Bottom
  {
    glm::vec3(1.0, 0.0, 0.0),
    glm::vec3(1.0, 0.0, 1.0),
    glm::vec3(0.0, 0.0, 1.0),
    glm::vec3(0.0, 0.0, 0.0),
  },                       
  // Top                   
  {                        
    glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(0.0, 0.0, 1.0),
    glm::vec3(1.0, 0.0, 1.0),
    glm::vec3(1.0, 0.0, 0.0),
  },                       
  // Left                  
  {                        
    glm::vec3(0.0, 0.0, 1.0),
    glm::vec3(0.0, 1.0, 1.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 0.0, 0.0),
  },                       
  // Right                 
  {                        
    glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 1.0),
    glm::vec3(0.0, 0.0, 1.0),
  },                       
  // Near
  {                        
    glm::vec3(0.0, 0.0,0.0),
    glm::vec3(0.0, 1.0,0.0),
    glm::vec3(1.0, 1.0,0.0),
    glm::vec3(1.0, 0.0,0.0),
  },                       
  // Far
  {                        
    glm::vec3(1.0, 0.0,0.0),
    glm::vec3(1.0, 1.0,0.0),
    glm::vec3(0.0, 1.0,0.0),
    glm::vec3(0.0, 0.0,0.0),
  },                       

  // Diag11
  {
    glm::vec3(1.0, 0.0, 1.0),
    glm::vec3(1.0, 1.0, 1.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 0.0, 0.0),
  },
  // Diag12
  {
    glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(1.0, 1.0, 1.0),
    glm::vec3(1.0, 0.0, 1.0),
  },
  // Diag21
  {
    glm::vec3(0.0, 0.0, 1.0),
    glm::vec3(0.0, 1.0, 1.0),
    glm::vec3(1.0, 1.0, 0.0),
    glm::vec3(1.0, 0.0, 0.0),
  },
  // Diag22
  {
    glm::vec3(1.0, 0.0, 0.0),
    glm::vec3(1.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 1.0),
    glm::vec3(0.0, 0.0, 1.0),
  },
};

}  // namespace face

void generateFace(
    // The buffer in which you want to store the face.
    std::vector<CubeVertex>& vArray,
    std::vector<CubeVertex>& alpha_vArray,
    std::uint8_t transparent,

    int tile,

    // The planetface coordinates.
    const glm::vec3& origin,
    const glm::vec3& v1,
    const glm::vec3& v2,

    // Position inside the planetface.
    const glm::vec3& position,

    int numBlocks,

    // Which type of face do you want.
    Face::Id face_id) {
  auto P = [&](const glm::vec3& p) {
    const glm::vec3 pp = (position + p);
    const glm::vec3 spherical_position =
        glm::normalize(origin + (v1 * pp.x + v2 * pp.z) / float(numBlocks));
    const float z = blockHeightToElevation(pp.y, numBlocks);
    return z * spherical_position;
  };

  const glm::vec3 face_position[4] = {
      P(Face::model[face_id][0]), P(Face::model[face_id][1]),
      P(Face::model[face_id][2]), P(Face::model[face_id][3]),
  };

  const glm::vec3 face_normal = glm::cross(face_position[2] - face_position[0],
                                           face_position[3] - face_position[1]);

  std::vector<CubeVertex>& va =
      transparent == blockTransparency::transparent ? alpha_vArray : vArray;
  va.push_back({face_position[0], face_normal, {0.0, 1.0}, tile + 0.5f});
  va.push_back({face_position[1], face_normal, {0.0, 0.0}, tile + 0.5f});
  va.push_back({face_position[2], face_normal, {1.0, 0.0}, tile + 0.5f});
  va.push_back({face_position[0], face_normal, {0.0, 1.0}, tile + 0.5f});
  va.push_back({face_position[2], face_normal, {1.0, 0.0}, tile + 0.5f});
  va.push_back({face_position[3], face_normal, {1.0, 1.0}, tile + 0.5f});
}

//TODO : optimiser pour éviter les multiplications à chaque fois
//(juste utiliser un pointeur à chaque fois...)
void computeChunkFaces(chunkVal* data,
		int w, int h, int d, //array sizes (in chunks)
		int sx, int sy, int sz, //chunk in array (in chunks)
		int px, int py, int pz, //chunk offset in world (in blocks)
		glm::vec3 origin, glm::vec3 v1, glm::vec3 v2,
		int numBlocks,
		std::vector<CubeVertex>& vArray, std::vector<CubeVertex>& alpha_vArray) //output
{
	vArray.clear();
	alpha_vArray.clear();
	//auto &blockType = BlockType::getInstance();

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
				generateFace(vArray, alpha_vArray, blockTransparencyID[current], getBlockID(current,blockPlane::side), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Left);
			}else if (blockShouldBeFace(previous,current)) {
				generateFace(vArray, alpha_vArray, blockTransparencyID[previous], getBlockID(previous,blockPlane::side), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Right);
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
				generateFace(vArray, alpha_vArray, blockTransparencyID[current], getBlockID(current,blockPlane::bottom), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Bottom);
			}else if (blockShouldBeFace(previous,current)) {
				generateFace(vArray, alpha_vArray, blockTransparencyID[previous], getBlockID(previous,blockPlane::top), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Top);
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
				generateFace(vArray, alpha_vArray, blockTransparencyID[current], getBlockID(current,blockPlane::side), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Near);
			}else if (blockShouldBeFace(previous,current)) {
				generateFace(vArray, alpha_vArray, blockTransparencyID[previous], getBlockID(previous,blockPlane::side), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Far);
			}
			previous=current;
		}
	}

	//sprites
	for(int x=0;x<CHUNK_N;++x)
	for(int y=0;y<CHUNK_N;++y)
	for(int z=0;z<CHUNK_N;++z) {
		current = accessArray(data,w,h,d,sx,sy,sz,x,y,z);
		if (blockStyleID[current] == blockStyle::sprite)
		{
			generateFace(vArray, alpha_vArray, blockTransparencyID[current], getBlockID(current,blockPlane::side), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Diag11);
			generateFace(vArray, alpha_vArray, blockTransparencyID[current], getBlockID(current,blockPlane::side), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Diag12);
			generateFace(vArray, alpha_vArray, blockTransparencyID[current], getBlockID(current,blockPlane::side), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Diag21);
			generateFace(vArray, alpha_vArray, blockTransparencyID[current], getBlockID(current,blockPlane::side), origin, v1, v2, vec3(px+x,py+y,pz+z), numBlocks, Face::Diag22);
		}
	}
}
