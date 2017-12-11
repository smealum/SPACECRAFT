#ifndef COMPRESSION_7BXZTDTZ
#define COMPRESSION_7BXZTDTZ

#include "Chunk.h"
#include <cstdint>

void chunkCompression(
		chunkVal* data,
		FILE* file
);

void chunkDecompression(
		FILE* file,
		chunkVal* data
);

#endif /* end of include guard: COMPRESSION_7BXZTDTZ */
