#ifndef CAMERAMANAGER_ASD83Q76
#define CAMERAMANAGER_ASD83Q76

#include "utils/glm.h"
#include <GLFW/glfw3.h>


class Camera;
// class patron
class CameraManager
{
    public:
        CameraManager();
        virtual void update(Camera& camera);
};


#endif /* end of include guard: CAMERAMANAGER_ASD83Q76 */
