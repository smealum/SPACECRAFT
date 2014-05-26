#ifndef PRODUCER_H
#define PRODUCER_H

#include <thread>
#include "ContentRequest.h"
#include "ContentReply.h"

class Producer
{
	public:
		Producer(ContentInputQueue& inputQueue, ContentOutputQueue& outputQueue);
	private:
		std::thread* thread;
		//FIFO synallagmatique
		ContentInputQueue& inputQueue;
		ContentOutputQueue& outputQueue;
};

#endif
