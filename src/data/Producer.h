#ifndef PRODUCER_H
#define PRODUCER_H

#include <SFML/System/Thread.hpp>
#include "data/ContentRequest.h"
#include "data/ContentReply.h"

class Producer
{
	public:
		Producer(int id, ContentInputQueue& inputQueue, ContentOutputQueue& outputQueue);
		void producerMain();
		~Producer();
	private:
		Producer();
		
		int id;
		sf::Thread *thread;
		//FIFO synallagmatique
		ContentInputQueue& inputQueue;
		ContentOutputQueue& outputQueue;
};

#endif
