#include "Planet.h"
#include "MiniWorld.h"
#include "data/ContentHandler.h"
#include "Application.h"
#include "utils/dbg.h"

//0-5-1
//|\ /|
//8-4-6
//|/ \|
//3-7-2

//[0][1]
//[2][3]

PlanetFace::PlanetFace(Planet* planet, glm::vec3 v[4]):
	planet(planet),
	father(NULL),
	sons{NULL, NULL, NULL, NULL},
	tptr(new TrackerPointer<PlanetFace>(this, true)),
	elevation(1.0f),
	elevated(false),
	id(5),
	bufferID(-1),
	miniworld(NULL),
	toplevel(this),
	x(0),
	z(0)
{
	uvertex[0]=v[0]; uvertex[1]=v[1];
	uvertex[2]=v[2]; uvertex[3]=v[3];
	depth=0;
	finalize();
}

PlanetFace::PlanetFace(Planet* planet, PlanetFace* father, uint8_t id):
	planet(planet),
	father(father),
	sons{NULL, NULL, NULL, NULL},
	tptr(new TrackerPointer<PlanetFace>(this, true)),
	elevation(1.0f),
	elevated(false),
	id(id),
	bufferID(-1),
	miniworld(NULL),
	toplevel(father->toplevel)
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
	if(!father)
	{
		for(int i=0;i<4;i++)if(sons[i])sons[i]->deletePlanetFace(b);
	}else{
		for(int i=0;i<4;i++)if(sons[i])sons[i]->deletePlanetFace(b);
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

void PlanetFace::updateElevation(float e)
{
	elevation=e;
	elevated=true;
}

#include <cstdio>

bool PlanetFace::shouldHaveMiniworld(Camera& c)
{
	// if(depth>13)printf("%f %f %f\n",vertex[4].x,vertex[4].y,vertex[4].z);
	return depth>=MINIWORLD_DETAIL;// && glm::length(c.getPosition()-vertex[4])<glm::length(vertex[1]-vertex[0])*5;
}

bool PlanetFace::isDetailedEnough(Camera& c)
{
	if(shouldHaveMiniworld(c))return true;
	if(depth>=MINIWORLD_DETAIL)return true;
	// if(depth>8)return true;
	glm::vec3 p1=c.getPosition();
	glm::vec3 p2=vertex[4]*elevation;
	glm::vec3 v=p2-p1;
	// if(glm::dot(v,vertex[4])>0.0f)return true; //backface culling
	// if(!c.isPointInFrustum(p2))return true; //frustum culling
	if(depth<4)return false;
	// float d=2.0f/(1<<(depth-3));
	float d=2.0f/(1<<(depth-2));
	if(glm::length(v)/d<1.f)return false;
	return true;
}

void PlanetFace::createMiniWorld(void)
{
	// if(miniworld || planet->numMiniWorlds()>=16)return;
	// if(miniworld || planet->numMiniWorlds()>=128)return;
	if(miniworld)return;

	miniworld=new MiniWorld(planet, this);
	planet->addMiniWorld(miniworld);
}

void PlanetFace::removeMiniWorld(void)
{
	if(!miniworld)return;

	planet->removeMiniWorld(miniworld);
	delete miniworld;
	miniworld=NULL;
}

void PlanetFace::processLevelOfDetail(Camera& c, PlanetFaceBufferHandler* b)
{
	if(isDetailedEnough(c))
	{
		for(int i=0;i<4;i++)if(sons[i])sons[i]->deletePlanetFace(b);
		if(elevated)
		{
			if(shouldHaveMiniworld(c))createMiniWorld();
			else removeMiniWorld();
			if(!miniworld)b->addFace(this);
		}
	}else{
		bool done=true;
		for(int i=0;i<4;i++)
		{
			if(sons[i])sons[i]->processLevelOfDetail(c, b);
			else sons[i]=new PlanetFace(planet,this,i);
			if(!sons[i]->elevated)done=false;
		}
		if(done)b->deleteFace(this);
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

Planet::Planet(planetInfo_s &pi, ContentHandler& ch):
	planetInfo(pi),
	handler(ch),
	programBasic(ShaderProgram::loadFromFile("shader/planet/planet.vert", "shader/planet/planet.frag", "planet")),
	generators(ch.getMaxProducers())
{
	for(int i=0;i<6;i++)faces[i]=new PlanetFace(this, cubeArray[i]);
	for(int i=0;i<6;i++)faceBuffers[i]=new PlanetFaceBufferHandler(*faces[i], 1024*16, cubeArray[i][1]-cubeArray[i][0], cubeArray[i][3]-cubeArray[i][0]);

	// for (size_t i = 0; i < ch.getMaxProducers(); i++)
	//     generators[i] = new PlanetGenerator(planetInfo);
	// log_info("generator: %f", generators[0]->getElevation(glm::vec3(-0.408248, -0.816497, -0.408248)));
	

	//TEMP pour drawDirect
	    glGenBuffers(1, &vbo);
	    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	    glGenBuffers(1, &ebo);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	    // basic programme configuration
	    {
			glGenVertexArrays(1, &vaoBasic);
			glBindVertexArray(vaoBasic);

			programBasic.setBuffers(vaoBasic, vbo, ebo);
				programBasic.use();
				glBindFragDataLocation(programBasic.getHandle(), 0, "outColor");
				programBasic.setAttribute("position", 3, GL_FALSE, 10, 0);
				programBasic.setAttribute("color", 4, GL_FALSE, 10, 3);
				// programBasic.setAttribute("texcoord", 2, GL_FALSE, 10, 0); // XXX pas de texcoord

				programBasic.setUniform("overrideColor", glm::vec4(1.f));

				programBasic.setUniform("model", glm::mat4(1.0f));
	    }
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

void PlanetFace::drawDirect(void)
{
	glm::quat q(glm::vec3(1.0,0.0,0.0),vertex[4]);
	if(sons[0])
	{
		for(int i=0;i<4;i++)
		{
			if(sons[i])sons[i]->drawDirect();
		}
	}else{
		float v=2.0f/(1<<depth);
	    planet->programBasic.setUniform("model", glm::translate(glm::mat4(1.0f),vertex[4]*elevation)*glm::scale(glm::mat4_cast(q),glm::vec3(v)));
        planet->programBasic.setUniform("overrideColor", glm::vec4(glm::vec3((elevation-1.0f)*10),1.0f));
	    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}

void Planet::drawDirect(void)
{
    Camera &cam(Application::getInstance().getCamera());
    // basic program
    {
        programBasic.use();
        glBindVertexArray(vaoBasic);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        cam.updateCamera(programBasic);

        // glm::mat4 model = glm::translate(glm::mat4(1.0),-glm::vec3(-1.0,0.0,0.0));
        // // model = glm::rotate(model,1.0f,glm::vec3(1.0,0.0,1.0));
        // programBasic.setUniform("model", model);

        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    	for(int i=0;i<6;i++)faces[i]->drawDirect();
    }
}

void Planet::processLevelOfDetail(Camera& c)
{
	for(int i=0;i<6;i++)faces[i]->processLevelOfDetail(c, faceBuffers[i]);
}

PlanetFaceBufferHandler::PlanetFaceBufferHandler(PlanetFace& pf, int ms, glm::vec3 v1, glm::vec3 v2):
	planetFace(pf),
	maxSize(ms),
	shader(ShaderProgram::loadFromFile("shader/planetface/planetface.vert", "shader/planetface/planetface.frag", "shader/planetface/planetface.geom", "planetface")),
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

	shader.setAttribute("position", 3, GL_FALSE, 6, 0);
	shader.setAttribute("elevation", 1, GL_FALSE, 6, 3);
	shader.setAttribute("minElevation", 1, GL_FALSE, 6, 4);
	shader.setAttribute("size", 1, GL_FALSE, 6, 5);

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
	buffer[i]=(faceBufferEntry_s){{n.x,n.y,n.z},pf->elevation,pf->elevation-0.05f,1.0f/(1<<pf->depth)};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, i*sizeof(faceBufferEntry_s), sizeof(faceBufferEntry_s), (void*)&buffer[i]);
}

void PlanetFaceBufferHandler::addFace(PlanetFace* pf)
{
	if(curSize>=maxSize || pf->bufferID>=0)return;
	changeFace(pf, curSize);
	pf->bufferID=curSize;
	curSize++;
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
}

void PlanetFaceBufferHandler::draw(Camera& c)
{
	shader.use();
	c.updateCamera(shader);

	shader.setUniform("v1", v1);
	shader.setUniform("v2", v2);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glDrawArrays(GL_POINTS, 0, curSize);
	// printf("%d, %d\n",curSize,faces.size());
}

void Planet::draw(Camera& c)
{
	for(int i=0;i<6;i++)faceBuffers[i]->draw(c);

	for(auto it(miniWorldList.begin()); it!=miniWorldList.end(); ++it)(*it)->draw(c);

	// printf("%d\n",miniWorldList.size());
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
