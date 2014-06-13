#include "Planet.h"
#include "MiniWorld.h"
#include "data/ContentHandler.h"
#include "Application.h"
#include "utils/dbg.h"
#include "glm/gtc/noise.hpp"
#include "world/BlockType.h"
#include "utils/positionMath.h"
#include "world/BlockType.h"


using namespace std;

//0-5-1
//|\ /|
//8-4-6
//|/ \|
//3-7-2

//[0][1]
//[2][3]

extern float PlanetFaceDetailsPower;

PlanetFace::PlanetFace(Planet* planet, glm::vec3 v[4], uint8_t id):
	planet(planet),
	father(NULL),
	faceBuffer(NULL),
	sons{NULL, NULL, NULL, NULL},
	tptr(new TrackerPointer<PlanetFace>(this, true)),
	elevation(1.0f),
	minElevation(elevation-0.01f),
	elevated(false),
	id(5+id),
	bufferID(-1),
	miniworld(NULL),
	toplevel(this),
	x(0),
	z(0),
	depth(0),
	childrenDepth(0),
	isDisplayOk(false),
	noBuffer(false)
{
	uvertex[0]=v[0]; uvertex[1]=v[1];
	uvertex[2]=v[2]; uvertex[3]=v[3];
	finalize();
}

PlanetFace::PlanetFace(Planet* planet, PlanetFace* father, uint8_t id):
	planet(planet),
	father(father),
	faceBuffer(NULL),
	sons{NULL, NULL, NULL, NULL},
	tptr(new TrackerPointer<PlanetFace>(this, true)),
	elevation(1.0f),
	elevated(false),
	id(id),
	bufferID(-1),
	miniworld(NULL),
	toplevel(father->toplevel),
	childrenDepth(0),
	isDisplayOk(false),
	noBuffer(false)
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
	if(faceBuffer)delete faceBuffer;
	removeMiniWorld();
}

void PlanetFace::deletePlanetFace(PlanetFaceBufferHandler* b)
{
	if(b)b->deleteFace(this);

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
	for(int i=0;i<4;i++)box[i]=vertex[i];
	for(int i=0;i<4;i++)box[i+4]=vertex[i]*1.1f;

	planet->handler.requestContent(new PlanetElevationRequest(*planet, *this, vertex[4]));
}

// mise a jour de l'elevation, de la température et de l'humidité
void PlanetFace::updateElevation(float e, blockTypes::T t)
{
	elevation=e;
	tile = t;
	minElevation=e-2.0f/(1<<depth);
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

	// if(depth<4)return false;

    // if(!c.isBoxInFrustum(box, 8, glm::mat4(1)))return true; //frustum culling
	
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

PlanetFace* PlanetFace::getTopLevel(void)
{
	return toplevel;
}

int PlanetFace::getID(void)
{
	return id;
}

static inline int max(int a, int b)
{
	return (a>b)?a:b;
}

void PlanetFace::processLevelOfDetail(Camera& c, PlanetFaceBufferHandler* b)
{
	bool detailedEnough=isDetailedEnough(c);

	// if(!faceBuffer)
	// {
	// 	if(sons[0])
	// 	{
	// 		bool allSons=true;
	// 		for(int i=0;allSons && i<4;i++)allSons=allSons && sons[i]->faceBuffer;

	// 		if(allSons)
	// 		{
	// 			int sum=0;
	// 			for(int i=0;i<4;i++)sum+=sons[i]->faceBuffer->getSize();
	// 			if(sum<PFBH_LOWTHRESHOLD)
	// 			{
	// 				noBuffer=false;
	// 				for(int i=0;i<4;i++)
	// 				{
	// 					delete sons[i]->faceBuffer;
	// 					sons[i]->faceBuffer=NULL;
	// 					sons[i]->noBuffer=false;
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	// if(faceBuffer)
	// {
	// 	if(b)printf("ERROR1\n");
	// 	b=faceBuffer;
	// 	if(faceBuffer->getSize()>PFBH_THRESHOLD && !detailedEnough)
	// 	{
	// 		printf("DELETE %d %d\n",depth,faceBuffer->getSize());
	// 		delete faceBuffer;
	// 		b=faceBuffer=NULL;
	// 		noBuffer=true;
	// 	}
	// }else if(!noBuffer){
	// 	if(!b)b=faceBuffer=new PlanetFaceBufferHandler(*this, PFBH_MAXSIZE, getV1(), getV2());
	// }

	// if(!b && !faceBuffer)
	// {
	// 	b=faceBuffer=new PlanetFaceBufferHandler(*this, PFBH_MAXSIZE, getV1(), getV2());
	// }else if(!b)
	// {
	// 	b=faceBuffer;
	// }

	if(faceBuffer)
	{
		if(b)printf("ERROR1 %d\n",depth);
		b=faceBuffer;
	}else if(!b && depth==2)b=faceBuffer=new PlanetFaceBufferHandler(*this, PFBH_MAXSIZE, getV1(), getV2());


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
	if(detailedEnough)
	{
		// dessin de la face
		if (elevated)
		{
			if(b)b->addFace(this); 
			else{printf("ERROR2 %d\n",depth);}

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
				if(b)b->deleteFace(this);
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
				if(!sons[i])sons[i]=new PlanetFace(planet,this,i);
				else sons[i]->processLevelOfDetail(c, b);

				done &= ( sons[i]->isDisplayOk );
			}

			// on peux ne plus afficher la face si les enfants affichent quelque chose.
			if (done)
			{
				if(b)b->deleteFace(this);
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
    //   POSITION    |      COLOR       |     NORMAL
    // x positif
    +0.0, -0.5, -0.5, 1.0, 0.0, 0.0, 1.f, +1.0, 0.0, 0.0,
    +0.0, +0.5, -0.5, 1.0, 0.0, 0.0, 1.f, +1.0, 0.0, 0.0,
    +0.0, +0.5, +0.5, 1.0, 0.0, 0.0, 1.f, +1.0, 0.0, 0.0,
    +0.0, -0.5, +0.5, 1.0, 0.0, 0.0, 1.f, +1.0, 0.0, 0.0,
};

static GLuint elements[2*3] = {
    0,1,2,      0,2,3, // face 1
};

Planet::Planet(PlanetInfo *pi, ContentHandler& ch, std::string name):
	planetInfo(pi),
	handler(ch),
	sunPosition(8.0,0.0,0.0),
	position(0.0,0.0,0.0),
	angle(0.0),
	name(name),
	atmosphere()
{	
	for(int i=0;i<6;i++)faces[i]=new PlanetFace(this, cubeArray[i], i);
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

//TEMP
extern bool testBool1;

void Planet::processLevelOfDetail(Camera& c)
{
	if(testBool1)return;
	if(!c.isBoxInFrustum(position-glm::vec3(1), glm::vec3(2,0,0), glm::vec3(0,2,0), glm::vec3(0,0,2)))return;
	if(glm::length(c.getPosition(position))>20.0f)return;
	for(int i=0;i<6;i++)faces[i]->processLevelOfDetail(c, NULL);
}

PlanetFaceBufferHandler::PlanetFaceBufferHandler(PlanetFace& pf, int ms, glm::vec3 v1, glm::vec3 v2):
	planetFace(pf),
	maxSize(ms),
	shader(ShaderProgram::loadFromFile("shader/planetface/planetface.vert", "shader/planetface/planetface.frag", "shader/planetface/planetface.geom", "planetface")),
	// shader(ShaderProgram::loadFromFile("shader/planetface_atmosphere/planetface_atmosphere.vert", "shader/planetface_atmosphere/planetface_atmosphere.frag", "shader/planetface_atmosphere/planetface_atmosphere.geom", "planetface_atmosphere")),
	curSize(0),
	curCapacity(PFBH_MINCAP),
	v1(glm::normalize(v1)),
	v2(glm::normalize(v2)),
	vbo(0),
	vao(0)
{
	resizeVBO();
	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "outColor");

	shader.setUniform("model", glm::mat4(1.0f));
}

PlanetFaceBufferHandler::~PlanetFaceBufferHandler()
{
	for(auto it=faces.begin();it!=faces.end();++it)(*it)->bufferID=-1;
	curCapacity=0;
	resizeVBO();
}

int PlanetFaceBufferHandler::getSize(void)
{
	return curSize;
}

void PlanetFaceBufferHandler::resizeVBO(void)
{
	// printf("RESIZE %d\n",curCapacity);
	if(vbo && vao && !curCapacity)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &vao);
		vbo=vao=0;
	}else if(curCapacity){
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, curCapacity*sizeof(faceBufferEntry_s), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, curSize*sizeof(faceBufferEntry_s), (void*)&buffer[0]);

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		shader.setBuffers(vao, vbo, 0);
		shader.use();

		shader.setAttribute("position", 3, GL_FALSE, 9, 0);
		shader.setAttribute("elevation", 1, GL_FALSE, 9, 3);
		shader.setAttribute("minElevation", 1, GL_FALSE, 9, 4);
		shader.setAttribute("size", 1, GL_FALSE, 9, 5);
		shader.setAttribute("topTile", 1, GL_FALSE, 9, 6);
		shader.setAttribute("sideTile", 1, GL_FALSE, 9, 7);
		shader.setAttribute("repeat", 1, GL_FALSE, 9, 8);
	}
}

//TODO : grouper les glBufferSubData de façon intelligente à chaque frame (glBufferSubData individuels pour les delete, mais groupé pour les add en queue ?)

void PlanetFaceBufferHandler::addFace(PlanetFace* pf)
{
	if(curSize>=maxSize || pf->bufferID>=0)return;

	if(curSize>=curCapacity)
	{
		while(curSize>=curCapacity)curCapacity*=2;
		resizeVBO();
	}

	const glm::vec3 n=pf->uvertex[4];

	int topTile = getBlockID(pf->tile,blockPlane::top);
	int sideTile = getBlockID(pf->tile,blockPlane::side);

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

	faces.push_back(pf);
	buffer.push_back((faceBufferEntry_s){{n.x,n.y,n.z},pf->elevation,pf->minElevation,1.0f/(1<<pf->depth),topTile,sideTile,repeat});

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, curSize*sizeof(faceBufferEntry_s), sizeof(faceBufferEntry_s), (void*)&buffer[curSize]);

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
	buffer.pop_back();
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
	shader.setUniform("planetPos", planetFace.planet->position-c.getReference());
	shader.setUniform("cameraPos", c.getPosition(planetFace.planet->position));
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

void PlanetFace::draw(Camera& c, glm::vec3 lightdir)
{
	if(!c.isBoxInFrustum(box, 8, glm::mat4(planet->getModel())))return; //frustum culling
	if(!faceBuffer || noBuffer)
	{
		if(sons[0])for(int i=0;i<4;i++)sons[i]->draw(c,lightdir);
	}else{
		faceBuffer->draw(c,lightdir);
	}
}


void Planet::draw(Camera& c)
{
	if(!c.isBoxInFrustum(position-glm::vec3(1), glm::vec3(2,0,0), glm::vec3(0,2,0), glm::vec3(0,0,2)))return;

	lightdir=glm::normalize(sunPosition-position);

	for(int i=0;i<6;i++)faces[i]->draw(c, lightdir);
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
	//TODO : pareil que collidePoint
	// printf("in %f %f %f\n",p.x,p.y,p.z);
	for(int i=0;i<3;i++)
	{
		for(auto it(miniWorldList.begin());it!=miniWorldList.end();++it)
		{
			glm::dvec3 out2=p;
			bool done;
			Chunk* ret=(*it)->selectBlock(p,v,out,out2,done,dir);
			if(ret)return ret;
			else if(done)return NULL;
			v-=out2-p;
			p=out2;
		}
	}
	// printf("out %f %f %f\n",p.x,p.y,p.z);
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
	if(planetInfo->trajectory)position=planetInfo->trajectory->getPosition(time);
	angle=time*2*PI/planetInfo->period;

	model=glm::mat3(glm::rotate(glm::mat4(1.0f),angle,planetInfo->axis));
	invModel=glm::transpose(model);
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

std::string Planet::getName(void)
{
	return name;
}
