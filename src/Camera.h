#ifndef __CAMERA_H__
#define __CAMERA_H__

#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>
#include "Shader.h"

class Camera {
    public:
        glm::mat4 view, proj;

        Camera();
        // the shader needs to have 2 uniforms: view and proj
        void updateCamera(ShaderProgram &prog);
};

#endif
