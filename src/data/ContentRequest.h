#ifndef CONTENTREQ_H
#define CONTENTREQ_H

#include "data/SynchronizationQueue.h"
#include "utils/TrackerPointer.h"

class ContentRequest
{
	public:
		virtual void process()=0;
		virtual void update()=0;
		virtual ~ContentRequest() {}
	private:		
};

#include "Planet.h"
#include "utils/glm.h"

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
		WorldChunkRequest(Planet& p, Chunk& c, glm::vec3 o, glm::vec3 e, int x, int y, int z);
		void process(void);
		void update(void);
		virtual ~WorldChunkRequest();
		
	private:
        char data[CHUNK_N][CHUNK_N][CHUNK_N];
        int px, py, pz;
		glm::vec3 origin, end;
		Planet& planet;
		TrackerPointer<Chunk>* chunk;
};

typedef SynchronizationQueue<ContentRequest*> ContentInputQueue;

#endif
