#ifndef CHUNKCACHE_H
#define CHUNKCACHE_H

#define CACHE_MAXSIZE (64)

#define CACHE_DIR "worlddata"

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
		ChunkCacheEntry(const std::string &name, FILE* f);

		void dump(void);

		bool shouldBeSaved(void);
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
		TrackerPointer<ChunkCacheEntry>* get(const std::string &name);
		void flush(void);

	private:
		// return true if the element was removed
		std::map<std::string,TrackerPointer<ChunkCacheEntry>* >::iterator removeChunk(std::map<std::string,TrackerPointer<ChunkCacheEntry>* >::iterator it);

		sf::Mutex mutex;
		std::map<std::string,TrackerPointer<ChunkCacheEntry>* > m_map;

};

#endif
