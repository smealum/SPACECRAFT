#include "../Camera.h"
#include "utils/Input.h"
#include <GLFW/glfw3.h>
#include "render/camera/CameraKeyboardMouse.h"
#include "render/camera/CameraPlayerGround.h"
#include "utils/maths.h"
#include "SolarSystem.h"

using namespace std;
using namespace glm;

extern SolarSystem* testSolarSystem;

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

    CameraKeyboard::update(camera);
    
    if(Input::isKeyPressed(GLFW_KEY_R))camera.setCameraManager(new CameraPlayerGround(testSolarSystem->getClosestPlanet(camera.getPosition(glm::vec3(0))))); //TODO : méga fuite à virer
}
