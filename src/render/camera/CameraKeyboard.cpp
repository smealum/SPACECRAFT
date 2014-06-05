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

float testAngle=0.0f;
extern Planet* testPlanet;

void CameraKeyboard::update(Camera& camera)
{
    float delta = Application::getInstance().getFrameDeltaTime();

    speedVect=vec3(0,0,0);

    // changement de la vitesse (manière brusque)
    if (Input::isKeyPressed(GLFW_KEY_Y))    speed*=10.0f;
    if (Input::isKeyPressed(GLFW_KEY_H))   speed/=10.0f;

    // changement de la vitesse (manière smooth)
    if (Input::isKeyHold(GLFW_KEY_T))    speed*=exp(delta*0.6);
    if (Input::isKeyHold(GLFW_KEY_G))   speed*=exp(-delta*0.6);

    // vitesse en translation
    float tS = speed * delta;

    // vitesse en rotation
    float rS = 1.5 * delta;

    // mode de précision (instantanée)
    if (Input::isKeyHold(GLFW_KEY_LEFT_SHIFT)){tS/=10.0f;rS=0.0003f;}
    if (Input::isKeyHold(GLFW_KEY_LEFT_CONTROL)){tS/=100.0f;rS=0.0003f;}
    
    if (Input::isKeyHold(GLFW_KEY_P))testAngle+=0.05f;
    if (Input::isKeyHold(GLFW_KEY_M))testAngle-=0.05f;

    // rotation
    if (Input::isKeyHold(GLFW_KEY_K))
        camera.view = rotate(mat4(1.0),rS,vec3(1.0,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_I))
        camera.view = rotate(mat4(1.0),rS,vec3(-1.0,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_J))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,-1.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_L))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,+1.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_U))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,0.0,-1.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_O))
        camera.view = rotate(mat4(1.0),rS,vec3(0.0,0.0,+1.0))*camera.view;

    // translation
    if (Input::isKeyHold(GLFW_KEY_A))
        speedVect+=vec3(+tS,0.0,0.0);
        // camera.view = translate(mat4(1.f), vec3(+tS,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_D))
        speedVect+=vec3(-tS,0.0,0.0);
        // camera.view = translate(mat4(1.0),vec3(-tS,0.0,0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_W))
        speedVect+=vec3(0,0.0,+tS);
        // camera.view = translate(mat4(1.0),vec3(0,0.0,+tS))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_S))
        speedVect+=vec3(0,0,-tS);
        // camera.view = translate(mat4(1.0),vec3(0,0,-tS))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_E))
        speedVect+=vec3(0.0,-tS, 0.0);
        // camera.view = translate(mat4(1.0),vec3(0.0,-tS, 0.0))*camera.view;
    if (Input::isKeyHold(GLFW_KEY_Q))
        speedVect+=vec3(0.0,+tS, 0.0);
        // camera.view = translate(mat4(1.0),vec3(0.0,+tS, 0.0))*camera.view;

    speedVect=speedVect*mat3(camera.view);

    //TEMP
    speedVect=camera.getPosition()-testPlanet->collidePoint(camera.getPosition(),-speedVect);
    
    camera.view = translate(camera.view,speedVect);
}
