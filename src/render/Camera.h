#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "render/Shader.h"
#include "utils/glm.h"

class CameraManager;

class Camera {
	public:
		Camera(float znear, float zfar);

		void update();
		void updateCamera(ShaderProgram &prog); // the shader needs to have 2 uniforms: view and proj
		void updateFrustum(void);
		
		void setCameraManager(CameraManager* c);
		glm::vec3 getPosition(void);

		bool isPointInFrustum(glm::vec3 p);
		bool isBoxInFrustum(glm::vec3 o, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
		
		glm::mat4 view, proj, final;

	private:
		glm::vec4 frustumPlane[6]; 
		float znear, zfar;

        CameraManager* cameraManager;
};

#endif
