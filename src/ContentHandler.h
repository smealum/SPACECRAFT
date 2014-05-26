#ifndef CONTENTHANDLER_H
#define CONTENTHANDLER_H

#include "Producer.h"
#include <vector>

class ContentHandler
{
	public:
		ContentHandler(int numProducers);
		void handleNewContent();
	private:
		std::vector<Producer>* producers;
        ContentInputQueue inputQueue;
        ContentOutputQueue outputQueue;
};

#endif
