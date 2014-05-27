#include "Planet.h"

//0-5-1
//|\ /|
//8-4-6
//|/ \|
//3-7-2

//[0][1]
//[2][3]

PlanetFace::PlanetFace(Planet* planet, glm::vec3 v[4]):
	planet(planet)
{
	vertex[0]=v[0]; vertex[1]=v[1];
	vertex[2]=v[2];	vertex[3]=v[3];
	finalize();
}

PlanetFace::PlanetFace(Planet* planet, PlanetFace* father, uint8_t id):
	planet(planet),
	father(father)
{
	//TODO : exception ?
	// if(!father);
	switch(id)
	{
		case 0:
			vertex[0]=father->vertex[0];
			vertex[1]=father->vertex[5];
			vertex[2]=father->vertex[4];
			vertex[3]=father->vertex[8];
			break;
		case 1:
			vertex[0]=father->vertex[5];
			vertex[1]=father->vertex[1];
			vertex[2]=father->vertex[6];
			vertex[3]=father->vertex[4];
			break;
		case 2:
			vertex[0]=father->vertex[8];
			vertex[1]=father->vertex[4];
			vertex[2]=father->vertex[3];
			vertex[3]=father->vertex[7];
			break;
		case 3:
			vertex[0]=father->vertex[4];
			vertex[1]=father->vertex[6];
			vertex[2]=father->vertex[2];
			vertex[3]=father->vertex[7];
			break;
		default:
			//TODO : exception ?
			break;
	}
	
	finalize();
}

void PlanetFace::finalize(void)
{
	vertex[4]=(vertex[0]+vertex[1]+vertex[2]+vertex[3])*0.25f;
	vertex[5]=(vertex[0]+vertex[1])*0.5f;
	vertex[6]=(vertex[1]+vertex[2])*0.5f;
	vertex[7]=(vertex[2]+vertex[3])*0.5f;
	vertex[8]=(vertex[3]+vertex[0])*0.5f;

	for(int i=0;i<9;i++)vertex[i]=glm::normalize(vertex[i]);
}

glm::vec3 cubeArray[6][4]=
		{{glm::vec3(-1.0,-1.0,-1.0),glm::vec3(-1.0,-1.0,1.0),glm::vec3(1.0,-1.0,1.0),glm::vec3(1.0,-1.0,-1.0)}, //bottom
		{glm::vec3(1.0,1.0,1.0),glm::vec3(-1.0,1.0,1.0),glm::vec3(-1.0,1.0,-1.0),glm::vec3(1.0,1.0,-1.0)}, //top
		{glm::vec3(-1.0,1.0,1.0),glm::vec3(-1.0,-1.0,1.0),glm::vec3(-1.0,-1.0,-1.0),glm::vec3(-1.0,1.0,-1.0)}, //left
		{glm::vec3(1.0,-1.0,-1.0),glm::vec3(1.0,-1.0,1.0),glm::vec3(1.0,1.0,1.0),glm::vec3(1.0,1.0,-1.0)}, //right
		{glm::vec3(1.0,1.0,-1.0),glm::vec3(-1.0,1.0,-1.0),glm::vec3(-1.0,-1.0,-1.0),glm::vec3(1.0,-1.0,-1.0)}, //near
		{glm::vec3(-1.0,-1.0,1.0),glm::vec3(-1.0,1.0,1.0),glm::vec3(1.0,1.0,1.0),glm::vec3(1.0,-1.0,1.0)}}; //far

Planet::Planet(planetInfo_s pi):
	planetInfo(pi)
{
	for(int i=0;i<6;i++)faces[i]=new PlanetFace(this, cubeArray[i]);
}
