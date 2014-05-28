#include "Planet.h"
#include "data/ContentHandler.h"
#include "Application.h"

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
	id(5)
{
	vertex[0]=v[0]; vertex[1]=v[1];
	vertex[2]=v[2];	vertex[3]=v[3];
	depth=0;
	finalize();
}

PlanetFace::PlanetFace(Planet* planet, PlanetFace* father, uint8_t id):
	planet(planet),
	father(father),
	sons{NULL, NULL, NULL, NULL},
	tptr(new TrackerPointer<PlanetFace>(this, true)),
	elevation(1.0f),
	id(id)
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
			vertex[2]=father->vertex[7];
			vertex[3]=father->vertex[3];
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
	
	depth=father->depth+1;
	finalize();
}

void PlanetFace::deletePlanetFace(void)
{
	if(!father)
	{
		for(int i=0;i<4;i++)if(sons[i])sons[i]->deletePlanetFace();
	}else{
		for(int i=0;i<4;i++)if(sons[i])sons[i]->deletePlanetFace();
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
	vertex[4]=(vertex[0]+vertex[1]+vertex[2]+vertex[3])*0.25f;
	vertex[5]=(vertex[0]+vertex[1])*0.5f;
	vertex[6]=(vertex[1]+vertex[2])*0.5f;
	vertex[7]=(vertex[2]+vertex[3])*0.5f;
	vertex[8]=(vertex[3]+vertex[0])*0.5f;

	for(int i=0;i<9;i++)vertex[i]=glm::normalize(vertex[i]);

	planet->handler.requestContent(new PlanetElevationRequest(*planet, *this, vertex[4]));
}

void PlanetFace::updateElevation(float e)
{
	elevation=e;
}

bool PlanetFace::isDetailedEnough(Camera& c)
{
	if(depth>9)return true;
	glm::vec3 p1=c.getPosition();
	glm::vec3 p2=vertex[4]*elevation;
	glm::vec3 v=p2-p1;
	// if(glm::dot(v,vertex[4])>0.0f)return true; //backface culling
	// if(!c.isPointInFrustum(p2))return true; //frustum culling
	if(depth<4)return false;
	float d=2.0f/(1<<(depth-3));
	if(glm::length(v)/d<1.f)return false;
	return true;
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

Planet::Planet(planetInfo_s pi, ContentHandler& ch):
	planetInfo(pi),
	handler(ch),
	programBasic(ShaderProgram::loadFromFile("shader/planet/planet.vert", "shader/planet/planet.frag", "planet"))
{
	for(int i=0;i<6;i++)faces[i]=new PlanetFace(this, cubeArray[i]);

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

void PlanetFace::processLevelOfDetail(Camera& c)
{
	if(isDetailedEnough(c))
	{
		for(int i=0;i<4;i++)if(sons[i])sons[i]->deletePlanetFace();
	}else{
		for(int i=0;i<4;i++)
		{
			if(sons[i])sons[i]->processLevelOfDetail(c);
			else sons[i]=new PlanetFace(planet,this,i);
		}
	}
}

void Planet::processLevelOfDetail(Camera& c)
{
	for(int i=0;i<6;i++)faces[i]->processLevelOfDetail(c);
}

PlanetFaceBufferHandler::PlanetFaceBufferHandler(PlanetFace& pf, int ms):
	planetFace(pf),
	maxSize(ms),
	shader(ShaderProgram::loadFromFile("shader/planetface/planetface.vert", "shader/planetface/planetface.frag", "planetface"))
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
	shader.setAttribute("color", 3, GL_FALSE, 6, 3);
	shader.setUniform("overrideColor", glm::vec4(1.f));
	shader.setUniform("model", glm::mat4(1.0f));
}

PlanetFaceBufferHandler::~PlanetFaceBufferHandler()
{
	free(buffer);
}

void PlanetFaceBufferHandler::changeFace(PlanetFace* pf, int i)
{
	if(i>=maxSize)return;
	faces.push_back(pf);
	const glm::vec3 v=pf->vertex[4]*pf->elevation;
	const glm::vec3 n=pf->vertex[4];
	buffer[i]=(faceBufferEntry_s){{v.x,v.y,v.z},{n.x,n.y,n.z}};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, i*sizeof(faceBufferEntry_s), sizeof(faceBufferEntry_s), (void*)&buffer[i]);
}

void PlanetFaceBufferHandler::addFace(PlanetFace* pf)
{
	changeFace(pf, curSize);
	curSize++;
}

void PlanetFaceBufferHandler::deleteFace(PlanetFace* pf, int i)
{
	if(i>=curSize || i<0)return;

	if(faces.size()>1)
	{
		const glm::vec3 v=pf->vertex[4]*pf->elevation;
		const glm::vec3 n=pf->vertex[4];
		faces[i]=faces[curSize-1];
		buffer[i]=buffer[curSize-1];

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, i*sizeof(faceBufferEntry_s), sizeof(faceBufferEntry_s), (void*)&buffer[i]);
	}

	faces.pop_back();
	curSize--;
}

void PlanetFaceBufferHandler::draw(Camera& c)
{
    shader.use();
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    c.updateCamera(shader);
	glDrawArrays(GL_POINTS, 0, curSize);
}
