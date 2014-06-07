#include "utils/stb_image.h"
#include <cstdlib>
#include "utils/TextureManager.h"
#include "utils/dbg.h"
#include "utils/gldbg.h"
#include <iostream>
using namespace std;

unsigned char * imageLoad(
		const std::string &filename,
		int& width,
		int& height,
		int &comp)
{
	cout<<"imageLoad[\""<<filename<<"\");"<<endl;
	unsigned char *data;

	FILE *file = fopen(filename.c_str(), "rb");
	if (!file)
		return 0;

	data = stbi_load_from_file(file, &width, &height, &comp, 0);

	cout<<"imageLoad[\""<<filename<<"\","<<(void*)data<<","<<width<<","<<height<<","<<comp<<"]"<<endl;
	fclose(file);

	return data;
}

void imageFree(unsigned char * data)
{
	if (data) stbi_image_free(data);
}

// load a 256x256 RGB .RAW file as a texture
GLuint TextureManager::loadTexture(const std::string& filename)
{
	glCheckError("Flush Previous Errors");

	auto it(textures.find(filename));
	if (it != textures.end())
		return it->second.id;

	GLuint texture;
	int width, height, comp;

	unsigned char * data=imageLoad(filename,width,height,comp);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, (comp==3)?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, data);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap?GL_REPEAT:GL_CLAMP);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap?GL_REPEAT:GL_CLAMP);
	// glBindTexture(GL_TEXTURE_2D,GL_NONE);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, (comp==4)?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, data);

	free(data);

	textures[filename].id = texture;
	textures[filename].width = width;
	textures[filename].height = height;

	glCheckError("Image load");

	return texture;
}

GLuint TextureManager::loadTextureArray(const std::string& filename, int tileW, int tileH)
{
	glCheckError("Flush Previous Errors");
	
	auto it(textures.find(filename));
	if (it != textures.end())
		return it->second.id;

	GLuint texture;
	int width, height, comp;

	unsigned char * data=imageLoad(filename,width,height,comp);

	// calcul du nombre de tiles
	int nxTile = width/tileW;
	int nyTile = height/tileH;
	int nTile = nxTile * nyTile;
	int tileSize = tileW * tileH;


	// changement de l'ordre des pixels dans la texture pour pouvoir utiliser les fonctions opengl
	unsigned char * dataGL = new unsigned char[4 * nTile * tileSize];
	//for each tile
	unsigned char * pixelGL = dataGL;
	for(int ty=0;ty<nyTile;++ty)
	for(int tx=0;tx<nxTile;++tx)
	{
		// for each pixel in the tile
		for(int y=0;y<tileW;++y)
		for(int x=0;x<tileH;++x)
		{
			*(pixelGL++) = data[4*( (y+ty*tileH)*width + (x+tx * tileW) ) +0]; // red
			*(pixelGL++) = data[4*( (y+ty*tileH)*width + (x+tx * tileW) ) +1]; // green
			*(pixelGL++) = data[4*( (y+ty*tileH)*width + (x+tx * tileW) ) +2]; // blue 
			*(pixelGL++) = data[4*( (y+ty*tileH)*width + (x+tx * tileW) ) +3]; // alpha
		}
	}
	
	log_info("TextureArray2D %d %d", nxTile,nyTile);
	log_info("TextureArray2D %dx%d", tileW, tileH);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
	// XXX review
	// J'ai remplacé glTexStorage3D par glTexImage3D
	glTexImage3D(
			GL_TEXTURE_2D_ARRAY,
			0, // mipmap level
			GL_RGBA, // format
			tileW, // tile width
			tileH, // tile height
			nTile, // number of tiles
			0,	   // border, must be zero
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			NULL
	);
	//glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tileW, tileH, nTile);
	glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY,
			0, // mipmap level
			0, // x in tile offset
			0, // y in tile offset
			0, // tile offset
			tileW, // tile width
			tileH, // tile height
			nTile, // number of tiles
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			dataGL
	);
	
	log_info("----");

	// texture interpolation
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_REPEAT);

	GLenum error = glGetError();
	log_info("%d",error);
	log_info("%d",GL_INVALID_ENUM);
	log_info("%d",GL_INVALID_VALUE);
	log_info("%d",GL_INVALID_FRAMEBUFFER_OPERATION);
	log_info("%d",GL_OUT_OF_MEMORY);


	free(data);
	delete[] dataGL;

	textures[filename].id = texture;
	textures[filename].width = tileW;
	textures[filename].height = tileH;

	glCheckError("Image Errors");

	return texture;
}

TextureManager::~TextureManager()
{
	for (auto it(textures.begin()); it != textures.end(); ++it) {
		glDeleteTextures(1, &it->second.id);
	}
}

void TextureManager::getTextureSize(const std::string& filename, int *width, int *height)
{
	loadTexture(filename);
	auto it(textures.find(filename));
	*width = it->second.width;
	*height = it->second.height;
}

