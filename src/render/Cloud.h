#ifndef CLOUD_5STCJ7YI
#define CLOUD_5STCJ7YI

#include "render/Shader.h"
#include "render/Camera.h"

class Cloud
{
	public:
		Cloud();
		void draw(Camera& c);
	private:
		ShaderProgram &shader;
		float time;
};

#endif /* end of include guard: CLOUD_5STCJ7YI */
