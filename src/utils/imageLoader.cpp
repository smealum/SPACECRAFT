#include "utils/stb_image.h"
#include <cstdlib>
#include <string>
#include <GL/glew.h>
#include <GL/gl.h>

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
GLuint loadTexture(const std::string& filename, bool wrap )
{
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

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, (comp==3)?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, data);

    free(data);

    return texture;
}
