#ifndef PRODUCER_H
#define PRODUCER_H

#ifndef __EMSCRIPTEN__
  #include <SFML/System/Thread.hpp>
#endif
#include "data/ContentRequest.h"

typedef SynchronizationQueue<ContentRequest*> ContentOutputQueue;

class Producer
{
	public:
		Producer(int id, ContentInputQueue& inputQueue, ContentOutputQueue& outputQueue);
		void producerMain();
		inline void endThread() { shouldEnd = true; } // ask the producer to end the thread
    bool ExecuteOneTask();
		~Producer();
	private:
		Producer();
		
		int id;
#ifndef __EMSCRIPTEN__
    sf::Thread *thread;
#endif
		//FIFO synallagmatique
		ContentInputQueue& inputQueue;
		ContentOutputQueue& outputQueue;
		bool shouldEnd; // stop the thread
};

#endif
