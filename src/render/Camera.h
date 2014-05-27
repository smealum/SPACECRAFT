#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "utils/glm.h"
#include "render/Shader.h"

class Camera {
	public:
		glm::mat4 view, proj, final;

		Camera();
		// the shader needs to have 2 uniforms: view and proj
		void updateCamera(ShaderProgram &prog);
		void updateFrustum(void);
	private:
		glm::vec4 frustumPlane[6]; 
};

#endif
