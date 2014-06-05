#ifndef SUN_Z7QDXCRK
#define SUN_Z7QDXCRK

#include "render/Shader.h"
#include "render/Camera.h"
#include "utils/glm.h"

class Sun
{
	public:
		Sun();
		void draw(Camera& c);
	private:
		ShaderProgram &shader;
		ShaderProgram &shaderGlow;
		float time;
		glm::vec3 position;
};

#endif /* end of include guard: SUN_Z7QDXCRK */
