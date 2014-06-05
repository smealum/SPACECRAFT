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

    speedVect=dvec3(0,0,0);

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

    // rotation
    if (Input::isKeyHold(GLFW_KEY_K))
        camera.view3 = mat3(rotate(mat4(1.0),rS,vec3(1.0,0.0,0.0)))*camera.view3;
    if (Input::isKeyHold(GLFW_KEY_I))
        camera.view3 = mat3(rotate(mat4(1.0),rS,vec3(-1.0,0.0,0.0)))*camera.view3;
    if (Input::isKeyHold(GLFW_KEY_J))
        camera.view3 = mat3(rotate(mat4(1.0),rS,vec3(0.0,-1.0,0.0)))*camera.view3;
    if (Input::isKeyHold(GLFW_KEY_L))
        camera.view3 = mat3(rotate(mat4(1.0),rS,vec3(0.0,+1.0,0.0)))*camera.view3;
    if (Input::isKeyHold(GLFW_KEY_U))
        camera.view3 = mat3(rotate(mat4(1.0),rS,vec3(0.0,0.0,-1.0)))*camera.view3;
    if (Input::isKeyHold(GLFW_KEY_O))
        camera.view3 = mat3(rotate(mat4(1.0),rS,vec3(0.0,0.0,+1.0)))*camera.view3;

    // translation
    if (Input::isKeyHold(GLFW_KEY_A))
        speedVect+=dvec3(+tS,0.0,0.0);
    if (Input::isKeyHold(GLFW_KEY_D))
        speedVect+=dvec3(-tS,0.0,0.0);
    if (Input::isKeyHold(GLFW_KEY_W))
        speedVect+=dvec3(0,0.0,+tS);
    if (Input::isKeyHold(GLFW_KEY_S))
        speedVect+=dvec3(0,0,-tS);
    if (Input::isKeyHold(GLFW_KEY_E))
        speedVect+=dvec3(0.0,-tS, 0.0);
    if (Input::isKeyHold(GLFW_KEY_Q))
        speedVect+=dvec3(0.0,+tS, 0.0);

    speedVect=(speedVect*glm::dmat3(camera.view3));

    // //TEMP
    // speedVect=camera.getPositionDouble()-testPlanet->collidePoint(camera.getPositionDouble(),-speedVect);
    
    camera.pos-=speedVect;
    
    camera.updateView();
    camera.updateFrustum();
}
