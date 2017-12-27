#include "Application.h"
#include "utils/Input.h"
#include "render/camera/CameraPlayerGround.h"
#include "render/camera/CameraKeyboardMouse.h"
#include "render/Cursor.h"
#include "utils/positionMath.h"
#include "Planet.h"
#include "MiniWorld.h"

//TEMP
extern Cursor* testCursor;

CameraPlayerGround::CameraPlayerGround(Planet* p, Camera& c, PlanetFace& pf):
	speedVect(0.0),
	planet(p),
	face(pf)
{
	localPosition=dspaceToBlock(glm::dmat3(planet->getInvModel())*c.getPositionDouble(glm::dvec3(planet->getPosition())),glm::dvec3(face.getOrigin()),glm::dvec3(face.getV1()),glm::dvec3(face.getV2()),glm::dvec3(face.getN()),planet->getNumBlocks());
	localView=glm::dmat3(1.0f);

	printf("%d %d %d\n",int(localPosition.x),int(localPosition.y),int(localPosition.z));

    c.moveReference(glm::dvec3(planet->getPosition()));
}

const glm::dvec3 playerBoundingBoxSize(0.4,2.0,0.4);

const glm::dvec3 playerBoundingBox[]=
{
	glm::dvec3(-1.0,-1.0,-1.0)*playerBoundingBoxSize,
	glm::dvec3(1.0,-1.0,-1.0)*playerBoundingBoxSize,
	glm::dvec3(1.0,-1.0,1.0)*playerBoundingBoxSize,
	glm::dvec3(-1.0,-1.0,1.0)*playerBoundingBoxSize,
	glm::dvec3(-1.0,0.20,-1.0)*playerBoundingBoxSize,
	glm::dvec3(1.0,0.20,-1.0)*playerBoundingBoxSize,
	glm::dvec3(1.0,0.20,1.0)*playerBoundingBoxSize,
	glm::dvec3(-1.0,0.20,1.0)*playerBoundingBoxSize
};

extern uint8_t selectBlockType;

void CameraPlayerGround::update(Camera& camera)
{
	int numBlocks=planet->getNumBlocks();
	//déplacements
	{
		float delta = Application::getInstance().getFrameDeltaTime();

		if(Input::isKeyPressed(GLFW_KEY_R)) {
      auto manager = new CameraKeyboardMouse; //TODO : méga fuite à virer
      manager->speed = 0.0001;
      camera.setCameraManager(manager);
    }

		const double tS=3.0*delta;
		const double gS=0.5*delta;
		const double jS=0.3;
		float rS=1.5*delta;

		if (Input::isKeyHold(GLFW_KEY_LEFT_SHIFT))rS/=20.0f;

		// rotation
		if (Input::isKeyHold(GLFW_KEY_K))localView = glm::dmat3(glm::mat3(glm::rotate(glm::mat4(1.0f),rS,glm::vec3(1.0,0.0,0.0))))*localView;
		if (Input::isKeyHold(GLFW_KEY_I))localView = glm::dmat3(glm::mat3(glm::rotate(glm::mat4(1.0f),rS,glm::vec3(-1.0,0.0,0.0))))*localView;
		if (Input::isKeyHold(GLFW_KEY_J))localView = glm::dmat3(glm::mat3(glm::rotate(glm::mat4(glm::mat3(localView)),rS,glm::vec3(0.0,-1.0,0.0))));
		if (Input::isKeyHold(GLFW_KEY_L))localView = glm::dmat3(glm::mat3(glm::rotate(glm::mat4(glm::mat3(localView)),rS,glm::vec3(0.0,+1.0,0.0))));

		//mouse
		if (Input::isMouseFixed())
		{
			if (Input::isKeyPressed(GLFW_KEY_T))Input::unfixMouse();
			// mouse speed
			const float s = 0.002;
			localView = glm::dmat3(glm::mat3(glm::rotate(glm::mat4(glm::mat3(localView)),s*Input::getHorAngle(),glm::vec3(0.0,-1.0,0.0))));
			localView = glm::dmat3(glm::mat3(glm::rotate(glm::mat4(1.0),s*Input::getVerAngle(),glm::vec3(-1.0,0.0,0.0))))*localView;
		}else if (Input::isKeyPressed(GLFW_KEY_T))Input::fixMouse();

		glm::dvec3 localSpeedVect;

		// translation
		if(Input::isKeyHold(GLFW_KEY_A))localSpeedVect+=glm::dvec3(-tS,0,0);
		if(Input::isKeyHold(GLFW_KEY_D))localSpeedVect+=glm::dvec3(tS,0,0);
		if(Input::isKeyHold(GLFW_KEY_W))localSpeedVect+=glm::dvec3(0,0.0,-tS);
		if(Input::isKeyHold(GLFW_KEY_S))localSpeedVect+=glm::dvec3(0,0,tS);

		localSpeedVect=(localSpeedVect*localView);

		glm::dvec3 g(0.0,-1.0,0.0);
		localSpeedVect-=g*glm::dot(g,localSpeedVect); //déplacements horizontaux uniquement

		if(Input::isKeyPressed(GLFW_KEY_SPACE))localSpeedVect-=g*jS; //saut

		speedVect+=localSpeedVect+g*gS; //gravité
		// speedVect+=localSpeedVect; //pas de gravité

		bool ret=false;
		for(int i=0;i<8;i++)
		{
			glm::dvec3 out;
			glm::dvec3 tempPos=localPosition+playerBoundingBox[i];
			ret=planet->collidePoint(tempPos,speedVect,out)||ret;
			speedVect=out-tempPos;
		}
		localPosition+=speedVect;

		//TODO : passer dans une methode de planet ?
		glm::dvec3 uPos=dblockToSpace(localPosition,glm::dvec3(face.getOrigin()),glm::dvec3(face.getV1()),glm::dvec3(face.getV2()),numBlocks);
		camera.setPosition(glm::dmat3(planet->getModel())*uPos+glm::dvec3(planet->getPosition()));
		
		glm::dvec3 mx=(dblockToSpace(localPosition+glm::dvec3(1.0,0.0,0.0),glm::dvec3(face.getOrigin()),glm::dvec3(face.getV1()),glm::dvec3(face.getV2()),numBlocks)-uPos);
		glm::dvec3 my=(dblockToSpace(localPosition+glm::dvec3(0.0,1.0,0.0),glm::dvec3(face.getOrigin()),glm::dvec3(face.getV1()),glm::dvec3(face.getV2()),numBlocks)-uPos);
		glm::dvec3 mz=(dblockToSpace(localPosition+glm::dvec3(0.0,0.0,1.0),glm::dvec3(face.getOrigin()),glm::dvec3(face.getV1()),glm::dvec3(face.getV2()),numBlocks)-uPos);
		my=glm::normalize(my);
		mx=glm::normalize(mx-my*glm::dot(mx,my));
		mz=glm::normalize(mz-my*glm::dot(mz,my)-mx*glm::dot(mz,mx));
		camera.view3=glm::mat3(localView)*glm::transpose(glm::mat3(mx,my,mz))*planet->getInvModel();

		if(ret)
		{
			//frottements sol
			double gval=glm::dot(g,speedVect);
			speedVect=(speedVect-gval*g)*0.5+gval*g;
		}else{
			double gval=glm::dot(g,speedVect);
			speedVect=(speedVect-gval*g)*0.7+gval*g;
		}

		camera.moveReference(glm::dvec3(planet->getPosition()));
		camera.updateView();
		camera.updateFrustum();
	}

	//selection
	{
		const double range=5.0;
		glm::i32vec3 out;
		glm::dvec3 v(-glm::transpose(localView)[2]);
		int dir;
		Chunk* ret=planet->selectBlock(localPosition, v*range, out, dir);

		// printf("v %f %f %f (%f)\n",v.x,v.y,v.z,glm::dot(v,g));
		if(ret)
		{
			// printf("%d block %d %d %d\n",dir,out.x,out.y,out.z);
			if(Input::isKeyPressed(GLFW_KEY_X) || Input::isMousePressed(GLFW_MOUSE_BUTTON_2))planet->deleteBlock(out);
			else if(Input::isKeyPressed(GLFW_KEY_C) || Input::isMousePressed(GLFW_MOUSE_BUTTON_1))
			{
				//TODO : check que player n'intersecte pas avec le nouveau bloc...
				blockTypes::T t=(blockTypes::T)selectBlockType;
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
			// testCursor->setPosition(out,dir,ret->origin,ret->v1,ret->v2,glm::translate(glm::mat4(1.0f),planet->getPosition()-camera.getReference())*glm::mat4(planet->getModel()),numBlocks);
			testCursor->setPosition(out,dir,ret->origin,ret->v1,ret->v2,glm::translate(glm::mat4(1.0f),planet->getPosition()-camera.getReference())*glm::mat4(planet->getModel()),numBlocks);
		}else{
			// printf("no block !\n");
			testCursor->unaffect();
		}
	}
}
