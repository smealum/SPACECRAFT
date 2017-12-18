#ifndef CONTENTREQ_H
#define CONTENTREQ_H

#include <vector>
#include <string>
#include "data/SynchronizationQueue.h"
#include "utils/TrackerPointer.h"
#include "utils/glm.h"

class ContentRequest
{
	public:
		ContentRequest();
		virtual bool isRelevant(int id);
		virtual void process(int id)=0;
		virtual void update()=0;
		virtual ~ContentRequest() {}
		bool isCanceled;
	private:
};

#include "Planet.h"

class PlanetElevationRequest : public ContentRequest
{
	public:
		PlanetElevationRequest(Planet& p, PlanetFace& pf, glm::vec3 c);
		void process(int id);
		void update(void);
		virtual ~PlanetElevationRequest();
		virtual bool isRelevant(int id);
		
	private:
		//bool isWater;
		blockTypes::T tile;
		float elevation;
		glm::vec3 coord;
		Planet& planet;
		TrackerPointer<PlanetFace>* face;
};

#include "Chunk.h"

#include "MiniWorld.h"

class MiniWorldDataRequest : public ContentRequest
{
	public:
		MiniWorldDataRequest(Planet& p, MiniWorld& mw, glm::vec3 o, glm::vec3 v1, glm::vec3 v2, int x, int y, int z, int numBlocks, ContentHandler& ch);
		void process(int id);
		void update(void);
		virtual ~MiniWorldDataRequest();
		virtual bool isRelevant(int id);
		
	private:
		chunkVal data[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D][(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)];
		int px, py, pz;
		glm::vec3 origin, v1, v2;
		int numBlocks;
		bool modified;
		std::string name;
		Planet& planet;
		TrackerPointer<MiniWorld>* miniworld;
		std::vector<CubeVertex> vArray[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D];
		std::vector<CubeVertex> alpha_vArray[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D];
		ContentHandler& contentHandler;
};

class MiniWorldDeletionRequest : public ContentRequest
{
	public:
		MiniWorldDeletionRequest(MiniWorld& mw, ContentHandler& ch);
		void process(int id);
		void update(void);
		virtual ~MiniWorldDeletionRequest();
		virtual bool isRelevant(int id);
		
	private:
		TrackerPointer<MiniWorld>* miniworld;
		ContentHandler& contentHandler;
};

#include "SolarSystem.h"

class SolarSystemDataRequest : public ContentRequest
{
	public:
		SolarSystemDataRequest(SolarSystem& ss, ContentHandler& ch);
		void process(int id);
		void update(void);
		virtual ~SolarSystemDataRequest();
		virtual bool isRelevant(int id);
		
	private:
		TrackerPointer<SolarSystem>* solarSystem;
		ContentHandler& contentHandler;

		std::vector<PlanetInfo*> planetInfos;

		float sunColor, sunSize;

		int seed;

		Sun* sun;
		Planet** planets;
};

typedef SynchronizationQueue<ContentRequest*> ContentInputQueue;

#endif
