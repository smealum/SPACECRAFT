#ifndef CAMERAKEYBOARD_FEF6DBSV
#define CAMERAKEYBOARD_FEF6DBSV

#include "CameraManager.h"

class CameraKeyboard : public CameraManager
{
    public:
        CameraKeyboard();
        virtual void update(Camera& camera);
};


#endif /* end of include guard: CAMERAKEYBOARD_FEF6DBSV */
