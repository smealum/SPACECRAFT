#include "data/Producer.h"
#include <stdlib.h>
#include <iostream>

void fakeProducerMain(Producer* p)
{
	p->producerMain();
}

Producer::Producer(ContentInputQueue& iq, ContentOutputQueue& oq):
	thread(new sf::Thread(&Producer::producerMain, this)),
	inputQueue(iq),
	outputQueue(oq)
{
    thread->launch();
}

Producer::~Producer()
{
    delete thread;
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
