#ifndef CONTENTHANDLER_H
#define CONTENTHANDLER_H

#include "data/Producer.h"
#include <vector>

class ContentHandler
{
	public:
		ContentHandler(int numProducers);

		void requestContent(ContentRequest* req);
		void handleNewContent();

		inline size_t getMaxProducers() const { return producers->size(); }
	private:

		std::vector<Producer*>* producers; // TODO is a pointer necessary
        ContentInputQueue inputQueue;
        ContentOutputQueue outputQueue;
};

#endif
