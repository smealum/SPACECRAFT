#ifndef CONTENTREP_H
#define CONTENTREP_H

#include "ContentRequest.h"
#include "SynchronizationQueue.h"

typedef SynchronizationQueue<ContentRequest*> ContentOutputQueue;

#endif
