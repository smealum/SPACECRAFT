#ifndef PRODUCER_H
#define PRODUCER_H

#include <SFML/System/Thread.hpp>
#include "data/ContentRequest.h"
#include "data/ContentReply.h"

class Producer
{
	public:
		Producer(ContentInputQueue& inputQueue, ContentOutputQueue& outputQueue);
		void producerMain();
		~Producer();
	private:
		Producer();
		sf::Thread *thread;
		//FIFO synallagmatique
		ContentInputQueue& inputQueue;
		ContentOutputQueue& outputQueue;
};

#endif
