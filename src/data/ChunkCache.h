#ifndef CHUNKCACHE_H
#define CHUNKCACHE_H

#define CACHE_MAXSIZE (64)

#include <SFML/System/Mutex.hpp>
#include <map>
#include <string>
#include <cstdio>
#include "utils/TrackerPointer.h"
#include "Chunk.h"
#include "MiniWorld.h"

class ChunkCacheEntry
{
	friend class ChunkCache;
	public:
		ChunkCacheEntry(MiniWorld* mw);
		ChunkCacheEntry(std::string name, FILE* f);

		void dump(void);

		chunkVal* getData(void);
		std::string getName(void);

	private:
		bool toSave;
		std::string name;
		chunkVal data[MINIWORLD_W][MINIWORLD_H][MINIWORLD_D][(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)];
};

class ChunkCache
{
	public:
		ChunkCache();

		void save(MiniWorld* mw);
		TrackerPointer<ChunkCacheEntry>* get(std::string name);
		void flush(void);

	private:
		void removeChunk(std::string name);

		sf::Mutex mutex;
		std::map<std::string,TrackerPointer<ChunkCacheEntry>*> map;

};

#endif
