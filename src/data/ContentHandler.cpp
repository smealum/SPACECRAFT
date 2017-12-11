#include <iostream>
#include "ContentHandler.h"
#include <queue>

ContentHandler::ContentHandler(int numProducers)
{
  for (int i = 0; i < numProducers; i++)
    producers.push_back(new Producer(i, inputQueue, outputQueue));
}

void ContentHandler::requestContent(ContentRequest* req, bool release)
{
	inputQueue.push(req,release);
}

void ContentHandler::manualReleaseInput(void)
{
	inputQueue.manualRelease();
}

//TEMP
#include "utils/Input.h"

void ContentHandler::handleNewContent(void)
{
	std::queue<ContentRequest*> q;
	outputQueue.popAll(q);

	while(q.size()>0)
	{
		ContentRequest* request=q.front();
		if(!request->isCanceled)
      request->update();
		q.pop();
		delete request;
	}

	if(Input::isKeyHold(GLFW_KEY_F))cache.flush();
}

bool ContentHandler::ExecuteOneTask() {
  return producers[0]->ExecuteOneTask();
}

ContentHandler::~ContentHandler() {
  // the producers MUST somehow terminate the thread
  for (auto it = producers.begin(); it != producers.end(); ++it)
    delete *it;
}
