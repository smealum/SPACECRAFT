#ifndef CONTENTHANDLER_H
#define CONTENTHANDLER_H

#include "data/ChunkCache.h"
#include "data/Producer.h"
#include <vector>

class ContentHandler {
  friend class MiniWorldDataRequest;
  friend class MiniWorldDeletionRequest;

public:
  ContentHandler(int numProducers);
  ~ContentHandler();

  void requestContent(ContentRequest *req, bool release = true);
  void manualReleaseInput(void);

  void handleNewContent();
  bool ExecuteOneTask();

  inline int getMaxProducers() const { return int(producers.size()); }

private:
  std::vector<Producer *> producers;
  ContentInputQueue inputQueue;
  ContentOutputQueue outputQueue;

  ChunkCache cache;
};

#endif
