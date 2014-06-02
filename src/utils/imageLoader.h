#ifndef IMAGELOADER_JSJVGSYT
#define IMAGELOADER_JSJVGSYT
#include <string>
#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

GLuint loadTexture(const std::string& filename, bool wrap);


unsigned char * imageLoad(const std::string &filename,int& width, int& height, int &comp);
void imageFree(unsigned char * data);


#endif /* end of include guard: IMAGELOADER_JSJVGSYT */

