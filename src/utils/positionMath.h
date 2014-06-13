#ifndef POSMATH_H
#define POSMATH_H

#include "MiniWorld.h"
#include "utils/glm.h"

//TODO : désinliner et passer dans un cpp ?

static inline float elevationToBlockHeight(float e, int numBlocks)
{
	return (e-1.0f)*numBlocks;
}

static inline float blockHeightToElevation(float y, int numBlocks)
{
	return 1.0f+(y/float(numBlocks));
}

static inline double delevationToBlockHeight(double e, int numBlocks)
{
	return (e-1.0)*numBlocks;
}

static inline double dblockHeightToElevation(double y, int numBlocks)
{
	return 1.0+(y/float(numBlocks));
}

static inline glm::vec3 spaceToBlock(glm::vec3 p, glm::vec3 origin, glm::vec3 v1, glm::vec3 v2, glm::vec3 n, int numBlocks)
{
	glm::vec3 blockPos;
	glm::vec3 unprojectedPos=(p*(glm::dot(origin,n)/glm::dot(p,n)))-origin;

	//calcul de la position en blocs dans la toplevel
	blockPos.x=(glm::dot(unprojectedPos,glm::normalize(v1))*numBlocks)/glm::length(v1);
	blockPos.y=elevationToBlockHeight(glm::length(p),numBlocks);
	blockPos.z=(glm::dot(unprojectedPos,glm::normalize(v2))*numBlocks)/glm::length(v2);

	return blockPos;
}

static inline glm::vec3 blockToSpace(glm::vec3 p, glm::vec3 origin, glm::vec3 v1, glm::vec3 v2, int numBlocks)
{
	glm::vec3 spacePos=origin+(v1*float(p.x)+v2*float(p.z))/float(numBlocks);
	spacePos=glm::normalize(spacePos);
	spacePos*=blockHeightToElevation(p.y,numBlocks);

	return spacePos;
}

static inline glm::dvec3 dspaceToBlock(glm::dvec3 p, glm::dvec3 origin, glm::dvec3 v1, glm::dvec3 v2, glm::dvec3 n, int numBlocks)
{
	glm::dvec3 blockPos;
	glm::dvec3 unprojectedPos=(p*(glm::dot(origin,n)/glm::dot(p,n)))-origin;

	//calcul de la position en blocs dans la toplevel
	blockPos.x=(glm::dot(unprojectedPos,glm::normalize(v1))*numBlocks)/glm::length(v1);
	blockPos.y=delevationToBlockHeight(glm::length(p), numBlocks);
	blockPos.z=(glm::dot(unprojectedPos,glm::normalize(v2))*numBlocks)/glm::length(v2);

	return blockPos;
}

static inline glm::dvec3 dblockToSpace(glm::dvec3 p, glm::dvec3 origin, glm::dvec3 v1, glm::dvec3 v2, int numBlocks)
{
	glm::dvec3 spacePos=origin+(v1*double(p.x)+v2*double(p.z))/double(numBlocks);
	spacePos=glm::normalize(spacePos);
	spacePos*=dblockHeightToElevation(p.y, numBlocks);

	return spacePos;
}

#endif
