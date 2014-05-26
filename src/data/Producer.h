#ifndef PRODUCER_H
#define PRODUCER_H

#include <thread>
#include "data/ContentRequest.h"
#include "data/ContentReply.h"

class Producer
{
	public:
		Producer(ContentInputQueue& inputQueue, ContentOutputQueue& outputQueue);
		void producerMain();
	private:
		std::thread* thread;
		//FIFO synallagmatique
		ContentInputQueue& inputQueue;
		ContentOutputQueue& outputQueue;
};

#endif
