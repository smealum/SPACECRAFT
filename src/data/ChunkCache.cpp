#include <cstdio>
#include "utils/Tools.h"
#include "data/ChunkCache.h"
#include "Compression.h"
#include "utils/dbg.h"

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

ChunkCacheEntry::ChunkCacheEntry(const std::string &name, FILE* f):
	name(name)
{
	toSave=false;
	chunkDecompression(f,(chunkVal*)data);
	//fread(data,1,sizeof(chunkVal)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*MINIWORLD_H*MINIWORLD_W*MINIWORLD_D,f);
}

chunkVal* ChunkCacheEntry::getData(void)
{
	return (chunkVal*)data;
}

bool ChunkCacheEntry::shouldBeSaved(void)
{
	return toSave;
}

std::string ChunkCacheEntry::getName(void)
{
	return name;
}


ChunkCache::ChunkCache()
{
	createDir(SAVE_DIR);
}

void ChunkCache::save(MiniWorld* mw)
{
	if(!mw)return;

	std::string name=mw->getName();
	printf("CACHING %s\n",name.c_str());

	mutex.lock();
		auto it=m_map.find(name);
		if(it!=m_map.end())
		{
			printf("TRYING TO CACHE ALREADY CACHED CHUNK\n");
            auto current = it++;
			removeChunk(current);
		}
		while(m_map.size()>CACHE_MAXSIZE)
			removeChunk(m_map.begin()); //TODO : système de prio (maintenir une queue de prio en parallèle ?)
		m_map[name] = new TrackerPointer<ChunkCacheEntry>(new ChunkCacheEntry(mw), true);
	mutex.unlock();
}

TrackerPointer<ChunkCacheEntry>* ChunkCache::get(const std::string &name)
{
	mutex.lock();
	auto it=m_map.find(name);
	if(it!=m_map.end())
	{
		TrackerPointer<ChunkCacheEntry>* tmp = it->second;
		m_map.erase(it);
		mutex.unlock();
		// it->second->grab();
		return tmp;
	}else{
		mutex.unlock();
		{
			std::string file(SAVE_DIR+std::string("/")+name);
			FILE* f=fopen(file.c_str(), "rb");

			if(f)
			{
				printf("LOADING %s (%p)\n",name.c_str(),f);
				TrackerPointer<ChunkCacheEntry>* ret=new TrackerPointer<ChunkCacheEntry>(new ChunkCacheEntry(name, f), true);
				fclose(f);
				return ret;
			}
		}
		return NULL;
	}
}

void ChunkCache::flush(void)
{
	mutex.lock();
	auto it = m_map.begin();
	while(it != m_map.end())
    {
        auto current = it++;
        removeChunk(current);
    }
	mutex.unlock();
}

void ChunkCache::removeChunk(std::map<std::string,TrackerPointer<ChunkCacheEntry>* >::iterator it)
{
	//suppose qu'on a déjà lock
	printf("FLUSHING %s\n",it->first.c_str());
	it->second->getPointer()->dump();
	it->second->release();
	m_map.erase(it);
}

void ChunkCacheEntry::dump(void)
{
	if(!toSave)return;

	std::string file(SAVE_DIR+std::string("/")+name);
	FILE* f=fopen(file.c_str(), "wb");

	printf("SAVING %s (%p)\n",name.c_str(),f);
	if(!f)return;

	chunkCompression((chunkVal*)data,f);

	fclose(f);
}
