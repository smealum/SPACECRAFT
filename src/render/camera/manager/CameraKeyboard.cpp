#include "../Camera.h"
#include "utils/Input.h"
#include "CameraKeyboard.h"
#include <GLFW/glfw3.h>
#include "utils/maths.h"

using namespace std;
using namespace glm;

CameraKeyboard::CameraKeyboard(int width, int height) : CameraManager(width, height)
{

}

void CameraKeyboard::update(Camera& camera)
{
    // vitesse en translation
    const float tS = 0.1;
    // vitesse en rotation
    const float rS = 0.03;

    // input from mouse
    vec3 dir, up, right;
    if (Input::fixMouse)
    {
	dir = vec3(
		cos(Input::getVerAngle()) * sin(Input::getHorAngle()),
		sin(Input::getVerAngle()),
		cos(Input::getVerAngle()) * cos(Input::getHorAngle())
		);
	vec3 right(
		sin(Input::getHorAngle() - M_PI/2.f),
		0.f,
		cos(Input::getHorAngle() - M_PI/2.f)
		);
	up = vec3(cross(right, dir));

	//camera.view = rotate(camera.view, Input::getHorAngle(), vec3(0.f, 1.f, 0.f));
    }
    //float delta = 1.f;

    // translation
    if (Input::isKeyHold(GLFW_KEY_A))
	//Input::position -= right * delta * tS;
	camera.view = translate(mat4(1.f), vec3(+tS,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_D))
	//Input::position += right * delta * tS;
	camera.view = translate(mat4(1.0),vec3(-tS,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_W))
	//Input::position += dir * delta * tS;
	camera.view = translate(mat4(1.0),vec3(0,0.0,+tS))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_S))
	//Input::position -= dir * delta * tS;
	camera.view = translate(mat4(1.0),vec3(0,0,-tS))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_Q))
	camera.view = translate(mat4(1.0),vec3(0.0,-tS, 0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_E))
	camera.view = translate(mat4(1.0),vec3(0.0,+tS, 0.0))*camera.view;

    // rotation
    if (Input::isKeyHold(GLFW_KEY_K))
        camera.view = rotate(mat4(1.0),rS,vec3(1.0,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_I))
        camera.view = rotate(mat4(1.0),rS,vec3(-1.0,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_J))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,-1.0,0.0))*camera.view;
        //camera.view = rotate(mat4(1.0),rS*Input::getHorAngle(),vec3(0.0,-1.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_L))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,+1.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_U))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,0.0,-1.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_O))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,0.0,+1.0))*camera.view;
    
    //camera.view = glm::lookAt(Input::position,
	    //Input::position+dir,
	    //up);

}
