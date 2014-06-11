#ifndef CHUNKCACHE_H
#define CHUNKCACHE_H

#define CACHE_MAXSIZE (64)

#include <map>
#include <string>
#include "Chunk.h"
#include "MiniWorld.h"

class ChunkCacheEntry
{
	public:
		ChunkCacheEntry(MiniWorld* mw);

		chunkVal* getData(void);
		std::string getName(void);

	private:
		std::string name;
		chunkVal data[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D][(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)];
};

class ChunkCache
{
	public:
		ChunkCache();

		void save(MiniWorld* mw);
		ChunkCacheEntry* get(std::string name);
		void flush(void);

	private:
		void removeChunk(std::string name);

		std::map<std::string,ChunkCacheEntry*> map;

};

#endif
