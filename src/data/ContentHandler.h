#ifndef CONTENTHANDLER_H
#define CONTENTHANDLER_H

#include "data/Producer.h"
#include <vector>

class ContentHandler
{
	public:
		ContentHandler(int numProducers);

		void requestContent(ContentRequest* req, bool release=true);
		void manualReleaseInput(void);

		void handleNewContent();

		inline int getMaxProducers() const { return int(producers.size()); }
	private:

		std::vector<Producer*> producers;
        ContentInputQueue inputQueue;
        ContentOutputQueue outputQueue;
};

#endif
