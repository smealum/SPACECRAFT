#include "../Camera.h"
#include "utils/Input.h"
#include <GLFW/glfw3.h>
#include "render/camera/CameraKeyboardMouse.h"
#include "render/camera/CameraPlayerGround.h"
#include "utils/maths.h"
#include "SolarSystem.h"
#include "galaxy/Galaxy.h"

using namespace std;
using namespace glm;

extern Galaxy* globalGalaxy;

CameraKeyboardMouse::CameraKeyboardMouse()
{

}

void CameraKeyboardMouse::update(Camera& camera)
{
    if (Input::isMouseFixed())
    {
        if (Input::isKeyPressed(GLFW_KEY_SPACE))Input::unfixMouse();

        // mouse speed
        const float s = 0.002;
        camera.view3 = mat3(rotate(mat4(1.0),s*Input::getHorAngle(),vec3(0.0,-1.0,0.0)))*camera.view3;
        camera.view3 = mat3(rotate(mat4(1.0),s*Input::getVerAngle(),vec3(-1.0,0.0,0.0)))*camera.view3;

    }else if (Input::isKeyPressed(GLFW_KEY_SPACE))Input::fixMouse();

    Planet* planet=globalGalaxy->getClosestPlanet(camera.getPosition(glm::vec3(0)));
    if(planet)camera.moveReference(glm::dvec3(planet->getPosition()));
    
    if(Input::isKeyPressed(GLFW_KEY_R))
   	{
   		//TODO : méga fuite à virer
   		if(planet)
   		{
   			PlanetFace& pf=planet->getTopLevelForCamera(camera);
   			camera.setCameraManager(new CameraPlayerGround(planet, camera, pf));
   		}
   	}

    CameraKeyboard::update(camera);
}
