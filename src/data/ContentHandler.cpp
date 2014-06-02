#include "ContentHandler.h"
#include <queue>

ContentHandler::ContentHandler(int numProducers)
{
	producers=new std::vector<Producer*>;
	for(int i=0;i<numProducers;i++)producers->push_back(new Producer(i, inputQueue, outputQueue));
}

void ContentHandler::requestContent(ContentRequest* req)
{
	inputQueue.push(req);
}

void ContentHandler::handleNewContent(void)
{
	std::queue<ContentRequest*> q;
	outputQueue.popAll(q);

	while(q.size()>0)
	{
		ContentRequest* r=q.front();
		r->update();
		q.pop();
		delete r;
	}
}
