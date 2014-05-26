#ifndef CONTENTREQ_H
#define CONTENTREQ_H

#include "data/SynchronizationQueue.h"

class ContentRequest
{
	public:
		virtual void process()=0;
	private:		
};

#include "Planet.h"
#include "utils/glm.h"

class PlanetElevationRequest : public ContentRequest
{
	public:
		PlanetElevationRequest(Planet& p, uint8_t f, glm::vec2 c);
		void process(void);
	private:
		uint8_t face;
		glm::vec2 coord;
		Planet& planet;
};

typedef SynchronizationQueue<ContentRequest*> ContentInputQueue;

#endif
