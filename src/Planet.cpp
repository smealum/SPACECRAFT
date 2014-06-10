#include "Planet.h"
#include "MiniWorld.h"
#include "data/ContentHandler.h"
#include "Application.h"
#include "utils/dbg.h"
#include "glm/gtc/noise.hpp"
#include "world/BlockType.h"
#include "utils/positionMath.h"


using namespace std;

//0-5-1
//|\ /|
//8-4-6
//|/ \|
//3-7-2

//[0][1]
//[2][3]

extern float PlanetFaceDetailsPower;

PlanetFace::PlanetFace(Planet* planet, glm::vec3 v[4]):
	planet(planet),
	father(NULL),
	sons{NULL, NULL, NULL, NULL},
	tptr(new TrackerPointer<PlanetFace>(this, true)),
	elevation(1.0f),
	temperature(0.0f),
	humidity(0.0f),
	minElevation(elevation-0.01f),
	elevated(false),
	id(5),
	bufferID(-1),
	miniworld(NULL),
	toplevel(this),
	x(0),
	z(0),
	depth(0),
	childrenDepth(0),
	isDisplayOk(false)
{
	uvertex[0]=v[0]; uvertex[1]=v[1];
	uvertex[2]=v[2]; uvertex[3]=v[3];
	finalize();
}

PlanetFace::PlanetFace(Planet* planet, PlanetFace* father, uint8_t id):
	planet(planet),
	father(father),
	sons{NULL, NULL, NULL, NULL},
	tptr(new TrackerPointer<PlanetFace>(this, true)),
	elevation(1.0f),
	temperature(0.0f),
	humidity(0.0f),
	elevated(false),
	id(id),
	bufferID(-1),
	miniworld(NULL),
	toplevel(father->toplevel),
	childrenDepth(0),
	isDisplayOk(false)
{
	//TODO : exception ?
	// if(!father);
	switch(id)
	{
		case 0:
			uvertex[0]=father->uvertex[0];
			uvertex[1]=father->uvertex[5];
			uvertex[2]=father->uvertex[4];
			uvertex[3]=father->uvertex[8];
			x=father->x*2+0; z=father->z*2+0;
			break;
		case 1:
			uvertex[0]=father->uvertex[5];
			uvertex[1]=father->uvertex[1];
			uvertex[2]=father->uvertex[6];
			uvertex[3]=father->uvertex[4];
			x=father->x*2+1; z=father->z*2+0;
			break;
		case 2:
			uvertex[0]=father->uvertex[8];
			uvertex[1]=father->uvertex[4];
			uvertex[2]=father->uvertex[7];
			uvertex[3]=father->uvertex[3];
			x=father->x*2+0; z=father->z*2+1;
			break;
		case 3:
			uvertex[0]=father->uvertex[4];
			uvertex[1]=father->uvertex[6];
			uvertex[2]=father->uvertex[2];
			uvertex[3]=father->uvertex[7];
			x=father->x*2+1; z=father->z*2+1;
			break;
		default:
			//TODO : exception ?
			break;
	}
	
	depth=father->depth+1;

	finalize();
}

PlanetFace::~PlanetFace()
{
	removeMiniWorld();
}

void PlanetFace::deletePlanetFace(PlanetFaceBufferHandler* b)
{
	b->deleteFace(this);

	// delete children
	for(int i=0;i<4;i++)if(sons[i])sons[i]->deletePlanetFace(b);

	// inform father
	if(father)
	{
		father->sons[id]=NULL;
		tptr->release();
	}
}

TrackerPointer<PlanetFace>* PlanetFace::getTptr(void)
{
	return tptr;
}

void PlanetFace::finalize(void)
{
	uvertex[4]=(uvertex[0]+uvertex[1]+uvertex[2]+uvertex[3])*0.25f;
	uvertex[5]=(uvertex[0]+uvertex[1])*0.5f;
	uvertex[6]=(uvertex[1]+uvertex[2])*0.5f;
	uvertex[7]=(uvertex[2]+uvertex[3])*0.5f;
	uvertex[8]=(uvertex[3]+uvertex[0])*0.5f;

	for(int i=0;i<9;i++)vertex[i]=glm::normalize(uvertex[i]);

	planet->handler.requestContent(new PlanetElevationRequest(*planet, *this, vertex[4]));
}

// mise a jour de l'elevation, de la température et de l'humidité
void PlanetFace::updateElevation(float e, float t, float h)
{
	elevation=e;
	minElevation=e-2.0f/(1<<depth);
	temperature=t;
	humidity=h;
	elevated=true;
}

#include <cstdio>

int randomSource=4;

bool PlanetFace::shouldHaveMiniworld(Camera& c)
{
	// if(planet->numMiniWorlds()>0 && !miniworld)return false; //TEMP DEBUG
	if (depth == MINIWORLD_DETAIL)
	{
		if (miniworld)
		{
			glm::vec3 p=planet->invModel*c.getPosition(planet->getPosition());
			return glm::length(vertex[4]*elevation-p)*(2<<(depth))<20.0f;
		}else{
			return (childrenDepth >= PLANET_ADDED_DETAIL);
		}
	}
	return false;
}

bool PlanetFace::isDetailedEnough(Camera& c)
{
	if(depth>MINIWORLD_DETAIL+PLANET_ADDED_DETAIL+1)return true;
	if(depth<4)return false;

	glm::vec3 p=planet->invModel*c.getPosition(planet->getPosition());
	if(glm::dot(vertex[0]*0.99f-p,vertex[0])>0.0f
	&& glm::dot(vertex[1]*0.99f-p,vertex[1])>0.0f
	&& glm::dot(vertex[2]*0.99f-p,vertex[2])>0.0f
	&& glm::dot(vertex[3]*0.99f-p,vertex[3])>0.0f
	&& glm::dot(vertex[4]*0.99f-p,vertex[4])>0.0f)return true; //backface culling
	
	//if (!sons[0])
		//if(!c.isPointInFrustum(vertex[4]))
			//return true; //frustum culling
	// float d=2.0f/(1<<(depth-3));
	//float d=2.0f/(1<<(depth-2));
	//if(length(vertex[4]*elevation-p)/d<1.2f)return false;
	// if(length(vertex[4]*elevation-p)*(2<<(depth))<40.0f) return false;
	if(glm::length(vertex[4]*elevation-p)*(2<<(depth-1))<PlanetFaceDetailsPower) return false;
	return true;
}

void PlanetFace::createMiniWorld(void)
{
	if(miniworld)return;

	miniworld=new MiniWorld(planet, this);
	planet->addMiniWorld(miniworld);
}

void PlanetFace::removeMiniWorld(void)
{
	if(!miniworld)return;

	planet->removeMiniWorld(miniworld);
	miniworld->destroyMiniWorld();
	miniworld=NULL;
}

static inline int max(int a, int b)
{
	return (a>b)?a:b;
}

void PlanetFace::processLevelOfDetail(Camera& c, PlanetFaceBufferHandler* b)
{
	// update childrenDepth
	childrenDepth = 0;
	for(int i=0;i<4;i++)
		if(sons[i])
			childrenDepth = max(childrenDepth,(sons[i]->childrenDepth + 1));

	// update isDisplayOk
	// isDisplayOk est vrai si on affiche une face ou bien un miniworld ou que tout les enfants 
	// sont présents et on isDisplayOk.
	isDisplayOk = isDrawingFace() || (miniworld && miniworld->isGenerated());
	if (!isDisplayOk)
	{
		isDisplayOk = true;
		for(int i=0;i<4;i++)
		{
			if( (!sons[i]) || !(sons[i]->isDisplayOk))
			{
				isDisplayOk = false;
				break;
			}
		}
	}


	// face assez détaillé, on l'affiche
	if(isDetailedEnough(c))
	{
		// dessin de la face
		if (elevated)
		{
			b->addFace(this); 

			// suppresion des éventuels miniWorlds si on a la face qui s'affiche
			removeMiniWorld();

			// suppression des éventuels enfants
			for(int i=0;i<4;i++)
				if(sons[i])
					sons[i]->deletePlanetFace(b);
		}
	}else{
		// creation/destruction du miniWorld
		if(shouldHaveMiniworld(c))
		{
			// creation du miniworld
			createMiniWorld();

			// effacement des enfants et de l'affichage de la face
			if(miniworld && miniworld->isGenerated())
			{
				// suppresion de la face
				b->deleteFace(this);
				// suppression des éventuels enfants
				for(int i=0;i<4;i++)
					if(sons[i])
						sons[i]->deletePlanetFace(b);
			}
			
		}else{

			// ajout des éventuels enfants
			bool done=true;
			for(int i=0;i<4;i++)
			{
				if(!sons[i])
					sons[i]=new PlanetFace(planet,this,i);
				else
					sons[i]->processLevelOfDetail(c, b);

				done &= ( sons[i]->isDisplayOk );
			}

			// on peux ne plus afficher la face si les enfants affichent quelque chose.
			if (done)
			{
				b->deleteFace(this);
				removeMiniWorld();
			}
		}

	}
}

glm::vec3 cubeArray[6][4]=
		{{glm::vec3(-1.0,-1.0,-1.0),glm::vec3(-1.0,-1.0,1.0),glm::vec3(1.0,-1.0,1.0),glm::vec3(1.0,-1.0,-1.0)}, //bottom
		{glm::vec3(1.0,1.0,1.0),glm::vec3(-1.0,1.0,1.0),glm::vec3(-1.0,1.0,-1.0),glm::vec3(1.0,1.0,-1.0)}, //top
		{glm::vec3(-1.0,1.0,1.0),glm::vec3(-1.0,-1.0,1.0),glm::vec3(-1.0,-1.0,-1.0),glm::vec3(-1.0,1.0,-1.0)}, //left
		{glm::vec3(1.0,-1.0,-1.0),glm::vec3(1.0,-1.0,1.0),glm::vec3(1.0,1.0,1.0),glm::vec3(1.0,1.0,-1.0)}, //right
		{glm::vec3(1.0,1.0,-1.0),glm::vec3(-1.0,1.0,-1.0),glm::vec3(-1.0,-1.0,-1.0),glm::vec3(1.0,-1.0,-1.0)}, //near
		{glm::vec3(-1.0,-1.0,1.0),glm::vec3(-1.0,1.0,1.0),glm::vec3(1.0,1.0,1.0),glm::vec3(1.0,-1.0,1.0)}}; //far

static GLfloat vertices[] = {
    //     POSITION    |      COLOR           |     NORMAL
    // x positif
    +0.0 , -0.5 , -0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    +0.0 , +0.5 , -0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    +0.0 , +0.5 , +0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    +0.0 , -0.5 , +0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
};

static GLuint elements[2*3] = {
    0,1,2,      0,2,3, // face 1
};

Planet::Planet(PlanetInfo &pi, ContentHandler& ch):
	planetInfo(pi),
	handler(ch),
	generators(ch.getMaxProducers()),
	sunPosition(8.0,0.0,0.0),
	position(0.0,0.0,0.0),
	axis(glm::normalize(glm::vec3(1.0,1.0,1.0))),
	angle(0.0),
	atmosphere()
{
	for(int i=0;i<ch.getMaxProducers();i++)generators[i] = new PlanetGenerator(planetInfo);
	
	for(int i=0;i<6;i++)faces[i]=new PlanetFace(this, cubeArray[i]);
	for(int i=0;i<6;i++)faceBuffers[i]=new PlanetFaceBufferHandler(*faces[i], PFBH_MAXSIZE, cubeArray[i][1]-cubeArray[i][0], cubeArray[i][3]-cubeArray[i][0]);
}

void PlanetFace::testFullGeneration(int depth, PlanetFaceBufferHandler* b)
{
	if(depth<=0)return;
	for(int i=0;i<4;i++)
	{
		if(!sons[i])sons[i]=new PlanetFace(planet,this,i);
		if(depth==1)b->addFace(sons[i]);
		sons[i]->testFullGeneration(depth-1, b);
	}
}

void Planet::testFullGeneration(int depth, PlanetFaceBufferHandler* b)
{
	for(int i=0;i<6;i++)faces[i]->testFullGeneration(depth, b);
	// faces[0]->deletePlanetFace();
}


void Planet::processLevelOfDetail(Camera& c)
{
	for(int i=0;i<6;i++)faces[i]->processLevelOfDetail(c, faceBuffers[i]);
}

PlanetFaceBufferHandler::PlanetFaceBufferHandler(PlanetFace& pf, int ms, glm::vec3 v1, glm::vec3 v2):
	planetFace(pf),
	maxSize(ms),
	shader(ShaderProgram::loadFromFile("shader/planetface/planetface.vert", "shader/planetface/planetface.frag", "shader/planetface/planetface.geom", "planetface")),
	// shader(ShaderProgram::loadFromFile("shader/planetface_atmosphere/planetface_atmosphere.vert", "shader/planetface_atmosphere/planetface_atmosphere.frag", "shader/planetface_atmosphere/planetface_atmosphere.geom", "planetface_atmosphere")),
	curSize(0),
	v1(glm::normalize(v1)),
	v2(glm::normalize(v2))
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, ms*sizeof(faceBufferEntry_s), NULL, GL_STATIC_DRAW);

    buffer=(faceBufferEntry_s*)malloc(ms*sizeof(faceBufferEntry_s));

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	shader.setBuffers(vao, vbo, 0);
	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "outColor");

	shader.setAttribute("position", 3, GL_FALSE, 9, 0);
	shader.setAttribute("elevation", 1, GL_FALSE, 9, 3);
	shader.setAttribute("minElevation", 1, GL_FALSE, 9, 4);
	shader.setAttribute("size", 1, GL_FALSE, 9, 5);
	shader.setAttribute("topTile", 1, GL_FALSE, 9, 6);
	shader.setAttribute("sideTile", 1, GL_FALSE, 9, 7);
	shader.setAttribute("repeat", 1, GL_FALSE, 9, 8);

	shader.setUniform("model", glm::mat4(1.0f));
}

PlanetFaceBufferHandler::~PlanetFaceBufferHandler()
{
	free(buffer);
}

//TODO : grouper les glBufferSubData de façon intelligente à chaque frame (glBufferSubData individuels pour les delete, mais groupé pour les add en queue ?)

void PlanetFaceBufferHandler::changeFace(PlanetFace* pf, int i)
{
	if(i>=maxSize)return;
	faces.push_back(pf);
	const glm::vec3 n=pf->uvertex[4];

	double block_height=delevationToBlockHeight(pf->elevation);

	int topTile,sideTile;
	if (block_height>double(CHUNK_N*MINIWORLD_H)*0.5) //  terre
	{
		// e > 0
		float e = (pf->elevation - 1.001) * 1000.f ;

		float sandCoef = 4.0*pf->temperature + 1.4*e - pf->humidity;
		float snowCoef = -2.0*pf->temperature+ 1.4*e + 0.3*pf->humidity;
		float stoneCoef = snowCoef+0.01 - 0.1*pf->humidity;
		float grassCoef = 0.05 + 2.0*abs(pf->humidity);

		// inihibition
		// (pas de sable près de l'eau)
		if (e<0.05) sandCoef = 0.0;
		// (A partir d'un moment la neige recouvre les cailloux
		stoneCoef = max(stoneCoef,0.8);

	
		// on choisit le plus grand
		int imax=0;int vmax=sandCoef;
		if (vmax<snowCoef) {vmax=snowCoef;imax=1;}
		if (vmax<stoneCoef) {vmax=stoneCoef;imax=2;}
		if (vmax<grassCoef) {vmax=grassCoef;imax=3;}
		
		
		switch(imax)
		{
			case 0:  topTile = blockTypes::sand-1;
			        sideTile = blockTypes::sand-1;
					break;
			case 1:  topTile = blockTypes::snow-1;
			        sideTile = blockTypes::snow-1;
					break;
			case 2:  topTile = blockTypes::stone-1;
			        sideTile = blockTypes::stone-1;
					break;
			case 3:  topTile = blockTypes::grass-1;
			        sideTile = blockTypes::grass-1;
					break;
		}

	}
	else // mer
	{
		// water
		topTile = blockTypes::water-1;
		sideTile = blockTypes::water-1;

		// sand
		//topTile = 18;
		//sideTile = 18;
	}

	float repeat;
	//if (pf->depth < MINIWORLD_DETAIL)
	//{
		//repeat = 1<<(MINIWORLD_DETAIL-(pf->depth));
	//}
	//else
	//{
		// // version 1
		//repeat = 1;

		// // version 2
		//int depth = pf->depth;
		//while(depth<MINIWORLD_DETAIL)
		//{
			//repeat*=0.5;
			//depth++;
		//}
	//}
	//repeat *= MINIWORLD_W * CHUNK_N;
	
	// peut-être que c'est mieux ainsi ?
	// j'attend des feedback (si quelqu'un passe par là)
	//-------------------------------------------------
	//
	// 
	//
	//
	//
	repeat = 2.0;
	

	buffer[i]=(faceBufferEntry_s){{n.x,n.y,n.z},pf->elevation,pf->minElevation,1.0f/(1<<pf->depth),topTile,sideTile,repeat};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, i*sizeof(faceBufferEntry_s), sizeof(faceBufferEntry_s), (void*)&buffer[i]);
}

void PlanetFaceBufferHandler::addFace(PlanetFace* pf)
{
	if(curSize>=maxSize || pf->bufferID>=0)return;
	changeFace(pf, curSize);
	pf->bufferID=curSize;
	curSize++;

	pf->isDisplayOk = true;
}

void PlanetFaceBufferHandler::deleteFace(PlanetFace* pf)
{
	const int i=pf->bufferID;
	if(i>=curSize || i<0)return;

	if(faces.size()>1)
	{
		faces[i]=faces[curSize-1];
		buffer[i]=buffer[curSize-1];

		faces[i]->bufferID=i;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, i*sizeof(faceBufferEntry_s), sizeof(faceBufferEntry_s), (void*)&buffer[i]);
	}

	pf->bufferID=-1;
	faces.pop_back();
	curSize--;
	pf->isDisplayOk = false;
}

// XXX TMP
extern int testTextureArray;

void PlanetFaceBufferHandler::draw(Camera& c, glm::vec3 lightdir)
{
	shader.use();
	c.updateCamera(shader);

	shader.setUniform("v1", v1);
	shader.setUniform("v2", v2);
	shader.setUniform("lightdir", lightdir);
	shader.setUniform("planetPos", planetFace.planet->position);
	shader.setUniform("model", glm::mat4(planetFace.planet->model));

	// //planetface_atmosphere test
	// planetFace.planet->atmosphere.bind(c,lightdir,shader);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);


	// bind la texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY,testTextureArray);
	shader.setUniform("Texture",0);

	glDrawArrays(GL_POINTS, 0, curSize);
	// printf("%d, %d\n",curSize,faces.size());
}


void Planet::draw(Camera& c)
{
	// TODO position de la planete
	lightdir=glm::normalize(sunPosition-position);

	for(int i=0;i<6;i++)faceBuffers[i]->draw(c, lightdir);

	for(auto it(miniWorldList.begin()); it!=miniWorldList.end(); ++it)(*it)->draw(c);

	// printf("%d\n",miniWorldList.size());
	
	// dessin de l'athmosphere
	atmosphere.draw(c, lightdir, position);

	// dessin des nuages
	// cloud.draw(c);
}

glm::vec3 Planet::getCameraRelativePosition(Camera& c)
{
	return invModel*c.getPosition(position);
}

glm::dvec3 Planet::getCameraRelativeDoublePosition(Camera& c)
{
	return glm::dmat3(invModel)*c.getPositionDouble(glm::dvec3(position));
}

PlanetFace& Planet::getTopLevelForCamera(Camera& c)
{
	glm::vec3 p=getCameraRelativePosition(c);
	for(int i=0;i<6;i++)
	{
		glm::vec3 p2=(p/fabs(glm::dot(faces[i]->getN(),p)))-faces[i]->getOrigin();
		float vv1=glm::dot(p2,faces[i]->getV1());
		float vv2=glm::dot(p2,faces[i]->getV2());
		if(fabs(glm::dot(faces[i]->getN(),p2))<=1e-6 && vv1>=0.0f && vv1<=4.0f && vv2>=0.0f && vv2<=4.0f)return *faces[i];
	}
	printf("ERROR ERROR\n");
	return *faces[0];
}

glm::mat3 Planet::getModel(void)
{
	return model;
}

void Planet::addMiniWorld(MiniWorld* mw)
{
	miniWorldList.push_back(mw);
}

void Planet::removeMiniWorld(MiniWorld* mw)
{
	miniWorldList.remove(mw);
}

int Planet::numMiniWorlds(void)
{
	return miniWorldList.size();
}

bool Planet::collidePoint(glm::dvec3 p, glm::dvec3 v, glm::dvec3& out)
{
	//TODO : vrai raymarching sur les miniworlds/chunks.
	//(mais en attendant, ça ça marche et bouffe pas tant que ça pour des faibles vitesses vu que de toute façon on cull les miniworlds)
	//(parce qu'on n'est pas complètement débile quand même, juste un peu flemmard)
	bool gret=false, ret;
	do{
		ret=false;
		for(auto it(miniWorldList.begin());it!=miniWorldList.end();++it)ret=ret||(*it)->collidePoint(p,v);
		gret=ret||gret;
	}while(ret && glm::length(v)>1e-6);
	out=p+v;
	return gret;
}

Chunk* Planet::selectBlock(glm::dvec3 p, glm::dvec3 v, glm::i32vec3& out, int& dir)
{
	for(auto it(miniWorldList.begin());it!=miniWorldList.end();++it)
	{
		Chunk* ret=(*it)->selectBlock(p,v,out,dir);
		if(ret)return ret;
	}
	return NULL;
}

glm::dvec3 Planet::getGravityVector(glm::dvec3 p)
{
	return glm::normalize(p-glm::dvec3(position));
}

glm::vec3 Planet::getPosition(void)
{
	return position;
}

void Planet::setSunPosition(glm::vec3 p)
{
	sunPosition=p;
}

//TODO : filtrer par toplevel (en pratique devrait pas être nécessaire, mais on sait jamais)
void Planet::changeBlock(glm::i32vec3 p, blockTypes::T v)
{
	for(auto it(miniWorldList.begin());it!=miniWorldList.end();++it)(*it)->changeBlock(p,v);
}

void Planet::deleteBlock(glm::i32vec3 p)
{
	changeBlock(p, blockTypes::air);
}

void Planet::update(float time)
{
	if(planetInfo.trajectory)position=planetInfo.trajectory->getPosition(time);
	angle=time*2*PI/planetInfo.period;

	model=glm::mat3(glm::rotate(glm::mat4(1.0f),angle,axis));
	invModel=glm::transpose(model);
}

float Planet::getTemperature(const glm::vec3& pos)
{
	float distanceToEquatorFactor = 1.0-2.0*abs(
			glm::dot(
				glm::normalize(pos),
				glm::normalize(axis)
		 ));
	float noise1 = glm::simplex(pos*10.f) * 0.5;
	float noise2 = glm::simplex(pos*100.f) * 0.05;

	return glm::clamp(distanceToEquatorFactor+noise1+noise2,-1.f,1.f);
}

float Planet::getHumidity(const glm::vec3& pos)
{
	float noise = glm::simplex(pos*2.f) *  1.4;
	
	return glm::clamp(noise,-1.f,1.f);
}
glm::vec3 PlanetFace::getOrigin(void)
{
	return uvertex[0];
}

glm::vec3 PlanetFace::getV1(void)
{
	return uvertex[1]-uvertex[0];
}

glm::vec3 PlanetFace::getV2(void)
{
	return uvertex[3]-uvertex[0];
}

glm::vec3 PlanetFace::getN(void)
{
	return vertex[4];
}
