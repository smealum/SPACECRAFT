#ifndef CAMERAKEYBOARD_FEF6DBSV
#define CAMERAKEYBOARD_FEF6DBSV

#include "render/CameraManager.h"

class CameraKeyboard : public CameraManager
{
    public:
        CameraKeyboard();
        virtual void update(Camera& camera);
    private:
    	float speed;
};


#endif /* end of include guard: CAMERAKEYBOARD_FEF6DBSV */
