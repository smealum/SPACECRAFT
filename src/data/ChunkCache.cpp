#include "data/ChunkCache.h"

ChunkCacheEntry::ChunkCacheEntry(MiniWorld* mw)
{
	name=mw->getName();
	toSave=mw->modified;

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
	printf("SAVING %s\n",name.c_str());

	mutex.lock();
		removeChunk(name);
		while(map.size()>CACHE_MAXSIZE)removeChunk(map.begin()->first); //TODO : système de prio (maintenir une queue de prio en parallèle ?)
		map.insert(std::pair<std::string,TrackerPointer<ChunkCacheEntry>*>(name, new TrackerPointer<ChunkCacheEntry>(new ChunkCacheEntry(mw), true)));
	mutex.unlock();
}

TrackerPointer<ChunkCacheEntry>* ChunkCache::get(std::string name)
{
	mutex.lock();
	auto it=map.find(name);
	if(it!=map.end())
	{
		map.erase(it);
		mutex.unlock();
		it->second->grab();
		return it->second;
	}else{
		mutex.unlock();
		return NULL;
	}
}

void ChunkCache::flush(void)
{
	mutex.lock();
		for(auto it=map.begin(); it!=map.end(); ++it)removeChunk(it->first);
	mutex.unlock();
}

void ChunkCache::removeChunk(std::string name)
{
	//suppose qu'on a déjà lock
	auto it=map.find(name);
	if(it!=map.end())
	{
		printf("REMOVING %s\n",it->first.c_str());
		it->second->release();
		map.erase(name);
	}
}
