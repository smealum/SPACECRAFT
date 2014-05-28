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

typedef SynchronizationQueue<ContentRequest*> ContentInputQueue;

#endif
