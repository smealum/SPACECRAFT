#include "../Camera.h"
#include "utils/Input.h"
#include "CameraKeyboardMouse.h"
#include "CameraPlayerGround.h"
#include <GLFW/glfw3.h>
#include "utils/maths.h"

using namespace std;
using namespace glm;

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
    
    if(Input::isKeyPressed(GLFW_KEY_R))camera.setCameraManager(new CameraPlayerGround()); //TODO : méga fuite à virer
}
