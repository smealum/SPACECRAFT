#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <map>
#include <string>
#include "utils/Singleton.h"

class TextureManager : public Singleton<TextureManager> {
	private:
		std::map<std::string, GLuint> textures;
		friend class Singleton<TextureManager>;
	public:
		~TextureManager();

		GLuint loadTexture(const std::string& filename);
};


#endif
