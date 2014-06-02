#include "data/Producer.h"
#include <stdlib.h>
#include <iostream>
// waiting function
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>

void fakeProducerMain(Producer* p)
{
	p->producerMain();
}

Producer::Producer(ContentInputQueue& iq, ContentOutputQueue& oq):
	thread(new sf::Thread(fakeProducerMain, this)),
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
    //TODO : mieux qu'avec un sleep.
	while(1)
	{
		ContentRequest* cr=inputQueue.pop();
		if(cr)
		{
			cr->process();
			outputQueue.push(cr);
		}
        else
        {
            sf::sleep(sf::milliseconds(10));
        }
	}
}
