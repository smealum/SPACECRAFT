#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "utils/glm.h"
#include "render/Shader.h"

class Camera {
	public:
		glm::mat4 view, proj, final;

		Camera();
		void updateCamera(ShaderProgram &prog); // the shader needs to have 2 uniforms: view and proj
		void updateFrustum(void);
		bool isPointInFrustum(glm::vec3 p);

	private:
		glm::vec4 frustumPlane[6]; 
};

#endif
