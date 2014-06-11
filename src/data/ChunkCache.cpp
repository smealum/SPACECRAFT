#include <cstdio>
#include "utils/Tools.h"
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

ChunkCacheEntry::ChunkCacheEntry(std::string name, FILE* f):
	name(name)
{
	toSave=false;
	fread(data,1,sizeof(chunkVal)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*MINIWORLD_H*MINIWORLD_W*MINIWORLD_D,f);
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
	createDir(CACHE_DIR);
}

void ChunkCache::save(MiniWorld* mw)
{
	if(!mw)return;

	std::string name=mw->getName();
	printf("CACHING %s\n",name.c_str());

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
		// it->second->grab();
		return it->second;
	}else{
		mutex.unlock();
		{
			//TEMP
			//char cwd[256];
			//getcwd(cwd,256);

			//chdir("./worlddata");
			std::string file(CACHE_DIR+std::string("/")+name);
			FILE* f=fopen(file.c_str(), "rb");
			//chdir(cwd);

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
		for(auto it=map.begin(); it!=map.end(); ++it)removeChunk(it->first);
	mutex.unlock();
}

void ChunkCache::removeChunk(std::string name)
{
	//suppose qu'on a déjà lock
	auto it=map.find(name);
	if(it!=map.end())
	{
		printf("FLUSHING %s\n",it->first.c_str());
		it->second->getPointer()->dump();
		it->second->release();
		map.erase(name);
	}
}

void ChunkCacheEntry::dump(void)
{
	if(!toSave)return;

	//TEMP
	//char cwd[256];
	//getcwd(cwd,256);

	//chdir("./worlddata");
	std::string file(CACHE_DIR+std::string("/")+name);
	FILE* f=fopen(file.c_str(), "wb");
	//chdir(cwd);

	printf("SAVING %s (%p)\n",name.c_str(),f);
	if(!f)return;

	fwrite(data,1,sizeof(chunkVal)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2)*MINIWORLD_H*MINIWORLD_W*MINIWORLD_D,f);

	fclose(f);
}
