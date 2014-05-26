#include "data/Producer.h"
#include <stdlib.h>
#include <iostream>

void producerMain(Producer& p)
{
	// while(1)std::cout << "hello \n";
}

Producer::Producer(ContentInputQueue& iq, ContentOutputQueue& oq):
	inputQueue(iq),
	outputQueue(oq),
	thread(new std::thread(producerMain, std::ref(*this)))
{
	
}
