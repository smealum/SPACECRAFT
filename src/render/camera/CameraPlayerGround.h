#ifndef CAMERAPLAYERGROUND_H
#define CAMERAPLAYERGROUND_H

#include "utils/glm.h"
#include "render/Camera.h"
#include "render/CameraManager.h"
#include "Planet.h"

class CameraPlayerGround : public CameraManager
{
	public:
		CameraPlayerGround(Planet* p);
        
        virtual void update(Camera& camera);

	private:
		glm::dvec3 speedVect;
		Planet* planet;
		
};

#endif
