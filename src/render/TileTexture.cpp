#include "TileTexture.h"
#include "Application.h"

void TileTexture::update()
{
    float delta = Application::getInstance().getFrameDeltaTime();
	time+=delta;
	int frame = int(time*10.0);
	current = textures[frame % 5];

	if (time>100000.0) time = 0.0;
}

GLuint TileTexture::get()
{
	return current;
}

void TileTexture::init()
{
	TextureManager& t = TextureManager::getInstance();
	textures[0] = t.loadTextureArray("data/blocksPackArray_1.png",16,16);
	textures[1] = t.loadTextureArray("data/blocksPackArray_2.png",16,16);
	textures[2] = t.loadTextureArray("data/blocksPackArray_3.png",16,16);
	textures[3] = t.loadTextureArray("data/blocksPackArray_4.png",16,16);
	textures[4] = t.loadTextureArray("data/blocksPackArray_5.png",16,16);
	time = 0.0;
}
