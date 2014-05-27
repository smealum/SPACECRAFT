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
    // vitesse en translation
    const float tS = 0.1;
    // vitesse en rotation
    const float rS = 0.03;

    // translation
    if (Input::isKeyHold(GLFW_KEY_A))
        camera.view = translate(mat4(1.0),vec3(+tS,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_D))
        camera.view = translate(mat4(1.0),vec3(-tS,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_W))
        camera.view = translate(mat4(1.0),vec3(0,0.0,+tS))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_S))
        camera.view = translate(mat4(1.0),vec3(0,0,-tS))*camera.view;
    // rotation
    if (Input::isKeyHold(GLFW_KEY_I))
        camera.view = rotate(mat4(1.0),rS,vec3(1.0,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_K))
        camera.view = rotate(mat4(1.0),rS,vec3(-1.0,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_J))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,-1.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_L))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,+1.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_U))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,0.0,-1.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_O))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,0.0,+1.0))*camera.view;
}
