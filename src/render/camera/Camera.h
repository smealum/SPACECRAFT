#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "render/Shader.h"
#include "utils/glm.h"


class CameraManager;

class Camera {
	public:
        void update();
		glm::mat4 view, proj, final;

		Camera();
        void setCameraManager(CameraManager* c);

		void updateCamera(ShaderProgram &prog); // the shader needs to have 2 uniforms: view and proj
		void updateFrustum(void);
		bool isPointInFrustum(glm::vec3 p);

	private:
		glm::vec4 frustumPlane[6]; 

        CameraManager* cameraManager;
};

#endif
