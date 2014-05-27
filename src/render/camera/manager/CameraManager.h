#ifndef CAMERAMANAGER_ASD83Q76
#define CAMERAMANAGER_ASD83Q76

#include "../Camera.h"
#include <GLFW/glfw3.h>
#include "utils/glm.h"

// class patron
class CameraManager
{
    public:
        CameraManager(Camera& camera);
        void update();
    private:
        Camera& m_camera;
};


#endif /* end of include guard: CAMERAMANAGER_ASD83Q76 */
