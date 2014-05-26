#ifndef CONTENTREP_H
#define CONTENTREP_H

#include "data/SynchronizationQueue.h"
#include "data/ContentRequest.h"

typedef SynchronizationQueue<ContentRequest*> ContentOutputQueue;

#endif
