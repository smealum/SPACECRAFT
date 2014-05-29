#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "render/Shader.h"
#include "utils/glm.h"


class CameraManager;

class Camera {
	public:
        void update();
		glm::mat4 view, proj, final;

		Camera(float znear, float zfar);
        void setCameraManager(CameraManager* c);

		void updateCamera(ShaderProgram &prog); // the shader needs to have 2 uniforms: view and proj
		void updateFrustum(void);
		bool isPointInFrustum(glm::vec3 p);
		glm::vec3 getPosition(void);

	private:
		glm::vec4 frustumPlane[6]; 
		float znear, zfar;

        CameraManager* cameraManager;
};

#endif
