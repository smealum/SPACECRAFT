#include "data/Producer.h"
#include <stdlib.h>
#include <iostream>
// waiting function
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include "utils/dbg.h"

void fakeProducerMain(Producer* p)
{
	p->producerMain();
}

Producer::Producer(int id, ContentInputQueue& iq, ContentOutputQueue& oq):
	thread(new sf::Thread(fakeProducerMain, this)),
	inputQueue(iq),
	outputQueue(oq),
	id(id),
	shouldEnd(false)
{
	thread->launch();
	debug("Thread started");
}

Producer::~Producer()
{
	//thread->terminate();
	endThread();
	delete thread;
}

void Producer::producerMain()
{
	//TODO : attente passive avec signaux
	//TODO : mieux qu'avec un sleep.
	while(!shouldEnd)
	{
		
		ContentRequest* cr=inputQueue.pop();
		if(cr)
		{
			if(cr->isRelevant(id))cr->process(id);
			else cr->isCanceled = true;
			outputQueue.push(cr);
		}else{
			sf::sleep(sf::milliseconds(10));
		}
	}
}
