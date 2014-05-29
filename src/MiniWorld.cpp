#include "MiniWorld.h"

MiniWorld::MiniWorld(Planet* p):
	planet(p)
{
	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_W;j++)
		{
			for(int k=0;k<MINIWORLD_W;k++)
			{
				chunks[i][j][k]=new Chunk(p);
			}
		}
	}
}

void MiniWorld::draw(Camera& c)
{
	//TODO : frustum culling !
	//TODO : occlusion culling ?
	for(int i=0;i<MINIWORLD_W;i++)
	{
		for(int j=0;j<MINIWORLD_W;j++)
		{
			for(int k=0;k<MINIWORLD_W;k++)
			{
				chunks[i][j][k]->draw(c);
			}
		}
	}
}
