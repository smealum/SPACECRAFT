#include "data/ChunkCache.h"

ChunkCacheEntry::ChunkCacheEntry(MiniWorld* mw)
{
	name=mw->getName();

	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_H;j++)
		{
			for(int k=0;k<MINIWORLD_D;k++)
			{
				memcpy(data[i][j][k],mw->chunks[i][j][k]->value,sizeof(chunkVal)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2));
			}
		}
	}
}

chunkVal* ChunkCacheEntry::getData(void)
{
	return (chunkVal*)data;
}

std::string ChunkCacheEntry::getName(void)
{
	return name;
}


ChunkCache::ChunkCache()
{

}

void ChunkCache::save(MiniWorld* mw)
{
	if(!mw)return;

	std::string name=mw->getName();
	removeChunk(name);

	while(map.size()>CACHE_MAXSIZE)removeChunk(map.begin()->first); //TODO : système de prio (maintenir une queue de prio en parallèle ?)

	map.insert(std::pair<std::string,ChunkCacheEntry*>(name,new ChunkCacheEntry(mw)));
}

ChunkCacheEntry* ChunkCache::get(std::string name)
{
	auto it=map.find(name);
	return it->second;
}

void ChunkCache::flush(void)
{
	for(auto it=map.begin(); it!=map.end(); ++it)removeChunk(it->first);
}

void ChunkCache::removeChunk(std::string name)
{
	auto it=map.find(name);
	delete it->second;
	map.erase(name);
}
