#include "ContentHandler.h"
#include <queue>

ContentHandler::ContentHandler(int numProducers)
{
	producers=new std::vector<Producer>(numProducers, Producer(inputQueue, outputQueue));
}

void ContentHandler::requestContent(ContentRequest* req)
{
	inputQueue.push(req);
}

void ContentHandler::handleNewContent(void)
{
	std::queue<ContentRequest*> q;
	outputQueue.popAll(q);
	ContentRequest* r=NULL;

	while((r=q.front()))
	{
		r->update();
		delete r;
		q.pop();
	}
}
