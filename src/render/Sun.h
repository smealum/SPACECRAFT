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
		void drawGlow(Camera& c);
		void setPosition(glm::vec3 position);
	private:
		ShaderProgram &shader;
		ShaderProgram &shaderGlow;
		float time;
		glm::vec3 position;
		int lod;
};

#endif /* end of include guard: SUN_Z7QDXCRK */
