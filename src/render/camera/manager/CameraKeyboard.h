#ifndef CAMERAKEYBOARD_95EYY2BL
#define CAMERAKEYBOARD_95EYY2BL

#include "CameraManager.h"

class CameraKeyboard : public CameraManager
{
    public:
        CameraKeyboard(int width, int height);
        virtual void update(Camera& camera);
};

#endif /* end of include guard: CAMERAKEYBOARD_95EYY2BL */
