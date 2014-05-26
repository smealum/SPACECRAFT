#ifndef __CAMERA_H__
#define __CAMERA_H__

#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>

class Camera {
    public:
        glm::mat4 view, proj;

        Camera();
};

#endif
