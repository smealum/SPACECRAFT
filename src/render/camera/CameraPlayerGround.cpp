#include "Application.h"
#include "utils/Input.h"
#include "render/camera/CameraPlayerGround.h"
#include "render/camera/CameraKeyboardMouse.h"
#include "Planet.h"
#include "MiniWorld.h"

//TEMP
extern Planet* testPlanet;

CameraPlayerGround::CameraPlayerGround():
	speedVect(0.0)
{

}

void CameraPlayerGround::update(Camera& camera)
{
    float delta = Application::getInstance().getFrameDeltaTime();

	if(Input::isKeyPressed(GLFW_KEY_R))camera.setCameraManager(new CameraKeyboardMouse()); //TODO : méga fuite à virer

	const double tS=1e-5*delta;
	const double gS=1e-5*delta;
	const double jS=1e-6;
    const float rS=1.5*delta;

    // rotation
	if (Input::isKeyHold(GLFW_KEY_K))camera.view3 = glm::mat3(glm::rotate(glm::mat4(1.0),rS,glm::vec3(1.0,0.0,0.0)))*camera.view3;
	if (Input::isKeyHold(GLFW_KEY_I))camera.view3 = glm::mat3(glm::rotate(glm::mat4(1.0),rS,glm::vec3(-1.0,0.0,0.0)))*camera.view3;
	if (Input::isKeyHold(GLFW_KEY_J))camera.view3 = glm::mat3(glm::rotate(glm::mat4(1.0),rS,glm::vec3(0.0,-1.0,0.0)))*camera.view3;
	if (Input::isKeyHold(GLFW_KEY_L))camera.view3 = glm::mat3(glm::rotate(glm::mat4(1.0),rS,glm::vec3(0.0,+1.0,0.0)))*camera.view3;

	glm::dvec3 localSpeedVect;

	// translation
	if(Input::isKeyHold(GLFW_KEY_A))localSpeedVect+=glm::dvec3(+tS,0.0,0.0);
	if(Input::isKeyHold(GLFW_KEY_D))localSpeedVect+=glm::dvec3(-tS,0.0,0.0);
	if(Input::isKeyHold(GLFW_KEY_W))localSpeedVect+=glm::dvec3(0,0.0,+tS);
	if(Input::isKeyHold(GLFW_KEY_S))localSpeedVect+=glm::dvec3(0,0,-tS);

	localSpeedVect=(localSpeedVect*glm::dmat3(camera.view3));

	glm::dvec3 g=testPlanet->getGravityVector(camera.pos);

	//on ajuste l'orientation pour s'aligner sur g
	float val=glm::dot(glm::dvec3(glm::transpose(camera.view3)[0]),g);
	if(fabs(val)>1e-6)camera.view3=glm::mat3(glm::rotate(glm::mat4(1.0),5.0f*val*delta,glm::vec3(0.0,0.0,1.0)))*camera.view3;

	localSpeedVect-=g*glm::dot(g,localSpeedVect); //déplacements horizontaux uniquement

	if(Input::isKeyPressed(GLFW_KEY_SPACE))localSpeedVect-=g*jS; //saut

	speedVect+=localSpeedVect+g*gS; //gravité

	glm::dvec3 tp=camera.getPositionDouble()-g*(1.0/PLANETFACE_BLOCKS);
	glm::dvec3 out;
	bool ret=testPlanet->collidePoint(tp,-speedVect,out);
	speedVect=tp-out;

	camera.pos-=speedVect;

	if(ret)speedVect/=2.0; //frottements sol
	else{
		double gval=glm::dot(g,speedVect);
		speedVect=(speedVect-gval*g)*0.8+gval*g*0.97;
	}

	camera.updateView();
	camera.updateFrustum();
}
