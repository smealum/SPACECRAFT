#include "../Camera.h"
#include "utils/Input.h"
#include "CameraKeyboard.h"
#include <GLFW/glfw3.h>
#include "utils/maths.h"
#include "Application.h"

using namespace std;
using namespace glm;

CameraKeyboard::CameraKeyboard():
    speed(5)
{

}

void CameraKeyboard::update(Camera& camera)
{
    // vitesse en translation
    float delta = Application::getInstance().getFrameDeltaTime();
    float tS = speed * delta;
    // vitesse en rotation
    float rS = 1.5 * delta;

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

    if (Input::isKeyHold(GLFW_KEY_LEFT_SHIFT)){tS/=10.0f;rS=0.0003f;}
    if (Input::isKeyHold(GLFW_KEY_LEFT_CONTROL)){tS/=100.0f;rS=0.0003f;}
    // translation
    if (Input::isKeyHold(GLFW_KEY_A))
	   camera.view = translate(mat4(1.f), vec3(+tS,0.0,0.0))*camera.view; //Input::position -= right * delta * tS;
    if (Input::isKeyHold(GLFW_KEY_D))
    	camera.view = translate(mat4(1.0),vec3(-tS,0.0,0.0))*camera.view; //Input::position += right * delta * tS;
    if (Input::isKeyHold(GLFW_KEY_W))
    	camera.view = translate(mat4(1.0),vec3(0,0.0,+tS))*camera.view; //Input::position += dir * delta * tS;
    if (Input::isKeyHold(GLFW_KEY_S))
    	camera.view = translate(mat4(1.0),vec3(0,0,-tS))*camera.view; //Input::position -= dir * delta * tS;
    if (Input::isKeyHold(GLFW_KEY_E))
	   camera.view = translate(mat4(1.0),vec3(0.0,-tS, 0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_Q))
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
    
    // speed
    if (Input::isKeyPressed(GLFW_KEY_Y))speed*=10.0f;
    else if (Input::isKeyPressed(GLFW_KEY_H))speed/=10.0f;
}
