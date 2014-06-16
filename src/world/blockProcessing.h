#ifndef BLOCKPROCESSING_H
#define BLOCKPROCESSING_H

#include <vector>
#include "Planet.h"
#include "Chunk.h"
#include "MiniWorld.h"
#include "utils/glm.h"
#include "world/BlockType.h"

#define accessArray(data, w, h, d, px, py, pz, i, j, k) (data)[((px)+(py)*(w)+(pz)*(w)*(h))*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)+((i)+1)+((j)+1)*(CHUNK_N+2)+((k)+1)*(CHUNK_N+2)*(CHUNK_N+2)]

void computeChunkFaces(chunkVal* data,
		int w, int h, int d, //array sizes (in chunks)
		int sx, int sy, int sz, //chunk in array (in chunks)
		int px, int py, int pz, //chunk offset in world (in blocks)
		glm::vec3 origin, glm::vec3 v1, glm::vec3 v2,
		int numBlocks,
		std::vector<GL_Vertex>& vArray, std::vector<GL_Vertex>& alpha_vArray); //output

#endif
