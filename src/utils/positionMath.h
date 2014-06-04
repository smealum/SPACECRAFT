#ifndef POSMATH_H
#define POSMATH_H

#include "utils/glm.h"

static inline glm::vec3 spaceToBlock(glm::vec3 p, glm::vec3 origin, glm::vec3 v1, glm::vec3 v2, glm::vec3 n)
{
	glm::vec3 blockPos;
	glm::vec3 unprojectedPos=(p*(glm::dot(origin,n)/glm::dot(p,n)))-origin;
	//calcul de la position en blocs dans la toplevel
	blockPos.x=(glm::dot(unprojectedPos,glm::normalize(v1))*PLANETFACE_BLOCKS)/glm::length(v1);
	blockPos.y=(glm::length(p)-1.0f)*PLANETFACE_BLOCKS;
	blockPos.z=(glm::dot(unprojectedPos,glm::normalize(v2))*PLANETFACE_BLOCKS)/glm::length(v2);
	return blockPos;
}

#endif
