#include "Application.h"
#include "utils/Input.h"
#include "render/camera/CameraPlayerGround.h"
#include "render/camera/CameraKeyboardMouse.h"
#include "render/Cursor.h"
#include "Planet.h"
#include "MiniWorld.h"

//TEMP
extern Cursor* testCursor;

CameraPlayerGround::CameraPlayerGround(Planet* p):
	speedVect(0.0),
	planet(p)
{

}

void CameraPlayerGround::update(Camera& camera)
{
	//déplacements
	{
		float delta = Application::getInstance().getFrameDeltaTime();

		if(Input::isKeyPressed(GLFW_KEY_R))camera.setCameraManager(new CameraKeyboardMouse()); //TODO : méga fuite à virer

		if(!planet)return;

		const double tS=1e-5*delta;
		const double gS=2e-6*delta;
		const double jS=4e-7;
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

		glm::dvec3 g=planet->getGravityVector(camera.pos);

		//on ajuste l'orientation pour s'aligner sur g
		float val=glm::dot(glm::dvec3(glm::transpose(camera.view3)[0]),g);
		if(fabs(val)>1e-6)camera.view3=glm::mat3(glm::rotate(glm::mat4(1.0),5.0f*val*delta,glm::vec3(0.0,0.0,1.0)))*camera.view3;

		localSpeedVect-=g*glm::dot(g,localSpeedVect); //déplacements horizontaux uniquement

		if(Input::isKeyPressed(GLFW_KEY_SPACE))localSpeedVect-=g*jS; //saut

		speedVect+=localSpeedVect+g*gS; //gravité

		glm::dvec3 tp=planet->getCameraRelativeDoublePosition(camera)-g*(1.0/PLANETFACE_BLOCKS);
		glm::dvec3 out;
		bool ret=planet->collidePoint(tp,-speedVect,out);
		speedVect=tp-out;

		camera.pos-=speedVect;

		if(ret)speedVect/=2.0; //frottements sol
		else{
			double gval=glm::dot(g,speedVect);
			speedVect=(speedVect-gval*g)*0.7+gval*g;
		}

		camera.updateView();
		camera.updateFrustum();
	}

	//selection
	{
		const double range=1e-5; //~3 blocks
		glm::dvec3 g=planet->getGravityVector(camera.getPositionDouble(glm::dvec3(0.0)));
		glm::i32vec3 out;
		glm::dvec3 v(-glm::transpose(camera.view3)[2]);
		int dir;
		Chunk* ret=planet->selectBlock(planet->getCameraRelativeDoublePosition(camera), v*range, out, dir);

		// printf("v %f %f %f (%f)\n",v.x,v.y,v.z,glm::dot(v,g));
		if(ret)
		{
			// printf("%d block %d %d %d\n",dir,out.x,out.y,out.z);
			if(Input::isKeyPressed(GLFW_KEY_X))planet->deleteBlock(out);
			else if(Input::isKeyPressed(GLFW_KEY_C))
			{
				//TODO : check que player n'intersecte pas avec le nouveau bloc...
				blockTypes::T t=blockTypes::dirt;
				// blockTypes::T t=blockTypes::water;
				switch(dir)
				{
					case 0: planet->changeBlock(out-glm::i32vec3(1,0,0),t); break;
					case 1: planet->changeBlock(out+glm::i32vec3(1,0,0),t); break;
					case 2: planet->changeBlock(out-glm::i32vec3(0,1,0),t); break;
					case 3: planet->changeBlock(out+glm::i32vec3(0,1,0),t); break;
					case 4: planet->changeBlock(out-glm::i32vec3(0,0,1),t); break;
					case 5: planet->changeBlock(out+glm::i32vec3(0,0,1),t); break;
				}
			}
			testCursor->setPosition(out,dir,ret->origin,ret->v1,ret->v2,glm::translate(glm::mat4(1.0f),planet->getPosition())*glm::mat4(planet->getModel()));
		}else{
			// printf("no block !\n");
			testCursor->unaffect();
		}
	}
}
