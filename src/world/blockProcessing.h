#ifndef BLOCKPROCESSING_H
#define BLOCKPROCESSING_H

#include <vector>

#include "Chunk.h"
#include "CubeVertex.h"
#include "utils/glm.h"

#define accessArray(data, w, h, d, px, py, pz, i, j, k) (data)[((px)+(py)*(w)+(pz)*(w)*(h))*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)+((i)+1)+((j)+1)*(CHUNK_N+2)+((k)+1)*(CHUNK_N+2)*(CHUNK_N+2)]

void computeChunkFaces(chunkVal* data,
		int w, int h, int d, //array sizes (in chunks)
		int sx, int sy, int sz, //chunk in array (in chunks)
		int px, int py, int pz, //chunk offset in world (in blocks)
		glm::vec3 origin, glm::vec3 v1, glm::vec3 v2,
		int numBlocks,
		std::vector<CubeVertex>& vArray, std::vector<CubeVertex>& alpha_vArray);

#endif
