#include "../Camera.h"
#include "utils/Input.h"
#include "CameraKeyboard.h"
#include <GLFW/glfw3.h>

using namespace std;
using namespace glm;

CameraKeyboard::CameraKeyboard()
{

}

void CameraKeyboard::update(Camera& camera)
{
    if (Input::isKeyHold(GLFW_KEY_Q))
        camera.view = glm::translate(camera.view,vec3(0.1,0.0,0.0));
    if (Input::isKeyHold(GLFW_KEY_D))
        camera.view = glm::translate(camera.view,vec3(-1.0,0.0,0.0));
}
