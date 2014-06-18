#include "../Camera.h"
#include "galaxy/Galaxy.h"
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

//demo stuff
glm::dvec3 target(0.0);
glm::dvec3 targets[8]={
    glm::dvec3(1091.146322, -0.065097, -1000.001511),
    glm::dvec3(1091.148543, 0.050548, -1000.087311),
    glm::dvec3(514.392157, 0.000000, -130.238961),
    glm::dvec3(-484.821664, 0.000000, -117.179801),
    glm::dvec3(4093957957.280227, 574915980.858341, 9849105513.134794),
    glm::dvec3(4093958623.051689, 574915558.085391, 9849105241.231743),
    glm::dvec3(16681286785.911911, -2319275960.656412, -8470569221.782546),
    glm::dvec3(0.000000, 0.000000, 0.000000),
};
bool targetMode=false;
extern Galaxy* globalGalaxy;

void CameraKeyboard::update(Camera& camera)
{
    float delta = Application::getInstance().getFrameDeltaTime();

    speedVect=dvec3(0,0,0);

    // changement de la vitesse (manière brusque)
    if (Input::isKeyPressed(GLFW_KEY_Y))speed*=10.0f;
    if (Input::isKeyPressed(GLFW_KEY_H))speed/=10.0f;

    // changement de la vitesse (manière smooth)
    if (Input::isKeyHold(GLFW_KEY_T))speed*=exp(delta*0.6);
    if (Input::isKeyHold(GLFW_KEY_G))speed*=exp(-delta*0.6);

    // vitesse en translation
    float tS = speed * delta;

    // vitesse en rotation
    float rS = 1.5 * delta;

    //target (pour demo)
    if(Input::isKeyPressed(GLFW_KEY_Z))targetMode^=1;

    if(targetMode)
    {
        glm::dvec3 v=globalGalaxy->getGlobalPosition(camera.getPositionDouble(target));
        glm::vec3 v1=glm::vec3(glm::normalize(v));
        glm::vec3 v2=glm::transpose(camera.view3)[2];
        glm::quat q(v1,v2);
        camera.view3*=glm::mat3_cast(q);
        double s=glm::length(v);
        if(tS>s)tS=s/100.0;
    }

    if(Input::isKeyHold(GLFW_KEY_LEFT_SHIFT))
    {
        if(Input::isKeyPressed(GLFW_KEY_1))targets[0]=globalGalaxy->getGlobalPosition(camera.getPositionDouble(glm::dvec3(0.0)));
        else if(Input::isKeyPressed(GLFW_KEY_2))targets[1]=globalGalaxy->getGlobalPosition(camera.getPositionDouble(glm::dvec3(0.0)));
        else if(Input::isKeyPressed(GLFW_KEY_3))targets[2]=globalGalaxy->getGlobalPosition(camera.getPositionDouble(glm::dvec3(0.0)));
        else if(Input::isKeyPressed(GLFW_KEY_4))targets[3]=globalGalaxy->getGlobalPosition(camera.getPositionDouble(glm::dvec3(0.0)));
        else if(Input::isKeyPressed(GLFW_KEY_5))targets[4]=globalGalaxy->getGlobalPosition(camera.getPositionDouble(glm::dvec3(0.0)));
        else if(Input::isKeyPressed(GLFW_KEY_6))targets[5]=globalGalaxy->getGlobalPosition(camera.getPositionDouble(glm::dvec3(0.0)));
        else if(Input::isKeyPressed(GLFW_KEY_7))targets[6]=globalGalaxy->getGlobalPosition(camera.getPositionDouble(glm::dvec3(0.0)));
        else if(Input::isKeyPressed(GLFW_KEY_8))targets[7]=globalGalaxy->getGlobalPosition(camera.getPositionDouble(glm::dvec3(0.0)));
    }else{
        if(Input::isKeyPressed(GLFW_KEY_1))target=targets[0];
        else if(Input::isKeyPressed(GLFW_KEY_2))target=targets[1];
        else if(Input::isKeyPressed(GLFW_KEY_3))target=targets[2];
        else if(Input::isKeyPressed(GLFW_KEY_4))target=targets[3];
        else if(Input::isKeyPressed(GLFW_KEY_5))target=targets[4];
        else if(Input::isKeyPressed(GLFW_KEY_6))target=targets[5];
        else if(Input::isKeyPressed(GLFW_KEY_7))target=targets[6];
        else if(Input::isKeyPressed(GLFW_KEY_8))target=targets[7];
    }

    if(Input::isKeyPressed(GLFW_KEY_9))
    {
        for(int i=0;i<8;i++)
        {
            printf("glm::dvec3(%f, %f, %f),\n",targets[i].x,targets[i].y,targets[i].z);
        }
    }

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

    camera.pos-=speedVect;
    
    camera.updateView();
    camera.updateFrustum();
}
