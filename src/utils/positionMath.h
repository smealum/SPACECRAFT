#ifndef POSMATH_H
#define POSMATH_H

#include "MiniWorld.h"
#include "utils/glm.h"

//TODO : désinliner et passer dans un cpp ?

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

static inline glm::vec3 blockToSpace(glm::vec3 p, glm::vec3 origin, glm::vec3 v1, glm::vec3 v2)
{
	glm::vec3 spacePos=origin+(v1*float(p.x)+v2*float(p.z))/float(PLANETFACE_BLOCKS);
	spacePos=glm::normalize(spacePos);
	spacePos*=1.0f+float(p.y)/float(PLANETFACE_BLOCKS);

	return spacePos;
}

static inline glm::dvec3 dspaceToBlock(glm::dvec3 p, glm::dvec3 origin, glm::dvec3 v1, glm::dvec3 v2, glm::dvec3 n)
{
	glm::dvec3 blockPos;
	glm::dvec3 unprojectedPos=(p*(glm::dot(origin,n)/glm::dot(p,n)))-origin;
	//calcul de la position en blocs dans la toplevel
	blockPos.x=(glm::dot(unprojectedPos,glm::normalize(v1))*PLANETFACE_BLOCKS)/glm::length(v1);
	blockPos.y=(glm::length(p)-1.0f)*PLANETFACE_BLOCKS;
	blockPos.z=(glm::dot(unprojectedPos,glm::normalize(v2))*PLANETFACE_BLOCKS)/glm::length(v2);
	return blockPos;
}

static inline glm::dvec3 dblockToSpace(glm::dvec3 p, glm::dvec3 origin, glm::dvec3 v1, glm::dvec3 v2)
{
	glm::dvec3 spacePos=origin+(v1*double(p.x)+v2*double(p.z))/double(PLANETFACE_BLOCKS);
	spacePos=glm::normalize(spacePos);
	spacePos*=1.0f+double(p.y)/double(PLANETFACE_BLOCKS);

	return spacePos;
}

#endif
