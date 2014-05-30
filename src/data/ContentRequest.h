#ifndef CONTENTREQ_H
#define CONTENTREQ_H

#include <vector>
#include "data/SynchronizationQueue.h"
#include "utils/TrackerPointer.h"
#include "utils/glm.h"

class ContentRequest
{
	public:
		virtual void process()=0;
		virtual void update()=0;
		virtual ~ContentRequest() {}
	private:		
};

#include "Planet.h"

class PlanetElevationRequest : public ContentRequest
{
	public:
		PlanetElevationRequest(Planet& p, PlanetFace& pf, glm::vec3 c);
		void process(void);
		void update(void);
		virtual ~PlanetElevationRequest();
		
	private:
		float elevation;
		glm::vec3 coord;
		Planet& planet;
		TrackerPointer<PlanetFace>* face;
};

#include "Chunk.h"

class WorldChunkRequest : public ContentRequest
{
	public:
		WorldChunkRequest(Planet& p, Chunk& c, glm::vec3 o, glm::vec3 v1, glm::vec3 v2, int x, int y, int z);
		void process(void);
		void update(void);
		virtual ~WorldChunkRequest();

		void computeChunk(void);
		
	private:
        char data[CHUNK_N][CHUNK_N][CHUNK_N];
        int px, py, pz;
		glm::vec3 origin, v1, v2;
		Planet& planet;
		TrackerPointer<Chunk>* chunk;
        std::vector<GL_Vertex> vArray;
};

typedef SynchronizationQueue<ContentRequest*> ContentInputQueue;

#endif
