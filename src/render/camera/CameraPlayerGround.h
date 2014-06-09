#ifndef CAMERAPLAYERGROUND_H
#define CAMERAPLAYERGROUND_H

#include "utils/glm.h"
#include "render/Camera.h"
#include "render/CameraManager.h"
#include "Planet.h"

class CameraPlayerGround : public CameraManager
{
	public:
		CameraPlayerGround(Planet* p, Camera& c, PlanetFace& pf);
        
        virtual void update(Camera& camera);

	private:
		glm::dmat3 localView;
		glm::dvec3 localPosition;
		glm::dvec3 speedVect;
		Planet* planet;
		PlanetFace& face;

};

#endif
