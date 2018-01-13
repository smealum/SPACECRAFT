#include "data/Producer.h"
#include <stdlib.h>
#include <iostream>
// waiting function
#ifndef __EMSCRIPTEN__
  #include <SFML/System/Sleep.hpp>
  #include <SFML/System/Time.hpp>
#endif
#include "utils/dbg.h"

void fakeProducerMain(Producer* p)
{
	p->producerMain();
}

Producer::Producer(int id, ContentInputQueue& iq, ContentOutputQueue& oq):
#ifndef __EMSCRIPTEN__
  thread(new sf::Thread(fakeProducerMain, this)),
#endif
	inputQueue(iq),
	outputQueue(oq),
	id(id),
	shouldEnd(false)
{
#ifndef __EMSCRIPTEN__
  thread->launch();
#endif
	debug("Thread started");
}

Producer::~Producer()
{
#ifndef __EMSCRIPTEN__
  thread->terminate();
  endThread();
  delete thread;
#endif
}

bool Producer::ExecuteOneTask() {
  ContentRequest* request;
  for (;;) {
    request = inputQueue.pop();

    if (!request)
      return false;

    if (!request->isRelevant(id)) {
      request->isCanceled = true;
      outputQueue.push(request);
      continue;
    }
    
    break;
  }
  request->process(id);
  outputQueue.push(request);
  return true;
}

void Producer::producerMain() {
#ifndef __EMSCRIPTEN__
  // TODO : attente passive avec signaux
  // TODO : mieux qu'avec un sleep.
  while (!shouldEnd) {
    if (!ExecuteOneTask())
      sf::sleep(sf::milliseconds(10));
  }
#endif
}
