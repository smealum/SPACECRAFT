#include "Producer.h"
#include <stdlib.h>
#include <iostream>

void fakeProducerMain(Producer& p)
{
	p.producerMain();
}

Producer::Producer(ContentInputQueue& iq, ContentOutputQueue& oq):
	inputQueue(iq),
	outputQueue(oq),
	thread(new std::thread(fakeProducerMain, std::ref(*this)))
{
	
}

void Producer::producerMain()
{
	//TODO : attente passive avec signaux
	while(1)
	{
		ContentRequest* cr=inputQueue.pop();
		if(cr)
		{
			cr->process();
			outputQueue.push(cr);
		}
	}
}
