#ifndef TILETEXTURE_U1KAUOTT
#define TILETEXTURE_U1KAUOTT

#include "utils/Singleton.h"
#include "utils/TextureManager.h"

class TileTexture : public Singleton<TileTexture>
{
	friend class Singleton<TileTexture>;
	public:
		void update();
		GLuint get();
		void init();
	private:
		float time;
		GLuint current;
		GLuint textures[5];
};

#endif /* end of include guard: TILETEXTURE_U1KAUOTT */
