#include "Galaxy.h"
#include <list>
#include "utils/dbg.h"
#include "SolarSystem.h"
#include "render/Shader.h"
#include "Planet.h"


using namespace glm;
using namespace std;

/////////////////////////////////////////////////////////
// paramètres de la galaxie
const double GALAXY_WIDTH = 2e10;
const dvec3 GALAXY_CENTER(0.0,0.0,0.0);
const double SOLARSYSTEM_MIN_DISTANCE = 0.001; 

/////////////////////////////////////////////////////////

Galaxy::Galaxy():
	time(0),
	galaxyTree(NULL),
	vao(0),
	vbo(0),
	program(ShaderProgram::loadFromFile(
		"shader/galaxy/galaxy.vert",
		"shader/galaxy/galaxy.frag",
		"galaxy")),
	isVBOGenerated(false),
	selectedPosition(NULL),
	currentSolarSystem(NULL)
{
	
}

Galaxy::~Galaxy()
{
	for(auto it = allocatedPositions.begin(); it!= allocatedPositions.end(); ++it)
	{
		delete *it;
	}

	if (vbo) glDeleteBuffers(1,&vbo);
	if (vao) glDeleteBuffers(1,&vao);
}

void Galaxy::pushSolarSystem(const dvec3& s)
{
	solarPosition.push_back(vec4(vec3(s),genrand64_real2()*6.2830));

	dvec3* solarSystem = new dvec3(s);
	int index=allocatedPositions.size();
	allocatedPositions.push_back(solarSystem);

	if (galaxyTree)
	{
		galaxyTree->pushSolarSystem(solarSystem, index);
	}else{
		galaxyTree = new GalaxyTree(solarSystem, index, GALAXY_CENTER, GALAXY_WIDTH);
	}
}


GalaxySolarResponse Galaxy::getClosestSolarSystem(const dvec3& pos , double maxDist)
{
	if (galaxyTree)
	{
		// test qu'on se trouve un peu dans la galaxie.
		dvec3 p = pos - GALAXY_CENTER;
		if (
				(p.x + maxDist > -GALAXY_WIDTH) and
				(p.x - maxDist <  GALAXY_WIDTH) and
				(p.y + maxDist > -GALAXY_WIDTH) and
				(p.y - maxDist <  GALAXY_WIDTH) and
				(p.z + maxDist > -GALAXY_WIDTH) and
				(p.z - maxDist <  GALAXY_WIDTH)
		)
		{
			return galaxyTree->getClosestSolarSystem(pos,maxDist);
		}
	}
	return {NULL, 2.0*maxDist, -1};
}

void Galaxy::generateVBO()
{
	isVBOGenerated = true;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*solarPosition.size(), &solarPosition[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindFragDataLocation(program.getHandle(), 0, "outColor");
	program.setAttribute("position", 3, GL_FALSE, 4, 0);
	program.setAttribute("offset", 1, GL_FALSE, 4, 3);

	program.setBuffers(vao, vbo, 0);
	program.use();
}

glm::dvec3 Galaxy::getGlobalPosition(glm::dvec3 p)
{
	if(!selectedPosition)return p;
	return p+*selectedPosition;
}

void Galaxy::step(Camera& camera, ContentHandler& contentHandler, float globalTime, float deltaTime)
{
	if (currentSolarSystem)
	{
		currentSolarSystem->update(globalTime);
	}
	
	time+=deltaTime*0.1f;

	// on saute des frames:
	{
		static const int nbSkippedFrame = 30;
		static int n = 0;
		if (n++%nbSkippedFrame) return;
	}
	
	dvec3 origin(0.0,0.0,0.0);
	dvec3 position = camera.getPositionDouble(origin);
	if(selectedPosition)position+=*selectedPosition;
	GalaxySolarResponse r = getClosestSolarSystem(position,1e7);

	// log_info("%f",r.distance);
	// return;

	// printf("DIST %f\n",glm::length(camera.getPositionDouble(origin)));

	if (r.solarSystem and r.distance<1e7)
	{
		dvec3 p = *(r.solarSystem);
		// log_info("Plus proche : (%f,%f,%f) = %f", p.x, p.y, p.z, r.distance);

		if (selectedPosition == r.solarSystem) return;

		log_info("%f",r.distance);
		printf("NEW NEW NEW %p\n",selectedPosition);

		if(selectedPosition)
			camera.movePositionDouble(*selectedPosition);

		// suppression du précédent système solaire.
		if (currentSolarSystem)
		{
			// TODO suppression du contenu généré
			currentSolarSystem->deleteSolarSystem();
			currentSolarSystem = NULL;
		}

		// ajout du nouveau système solaire.
		selectedPosition = r.solarSystem;

		int seed=r.index;
		printf("SEED %d\n",seed);
		currentSolarSystem = new SolarSystem(p, seed);
		currentSolarSystem->generate(contentHandler);

		camera.movePositionDouble(-*selectedPosition);

	}else{
		// suppression de la précédente position (et remise en place de la position)
		// log_info("Trop loin");
		if(selectedPosition)
		{
			camera.movePositionDouble(*selectedPosition);
			selectedPosition = NULL;
		}

		// suppression du précédent système solaire.
		if (currentSolarSystem)
		{
			// TODO suppression du contenu généré
			currentSolarSystem->deleteSolarSystem();
			currentSolarSystem = NULL;
		}
	}

}


void Galaxy::draw(Camera& camera)
{
	if (not isVBOGenerated) generateVBO();
    program.use();

    camera.updateCamera(program);

    if(selectedPosition)program.setUniform("model",glm::translate(glm::mat4(1.0f),glm::vec3(-*selectedPosition)-camera.getReference()));
    else program.setUniform("model",glm::translate(glm::mat4(1.0f),-camera.getReference()));
    
    program.setUniform("t",time);

    glDrawArrays(GL_POINTS, 0, solarPosition.size());
	
	
	// affichage du système solaire le plus proche
	if (currentSolarSystem)
	{
		currentSolarSystem->draw(camera);
	}
}

Planet* Galaxy::getClosestPlanet(const glm::vec3& pos)
{
	if (currentSolarSystem)
	{
		return currentSolarSystem->getClosestPlanet(pos);
	}
	return NULL;
}


////////////////////////////////////////////////////////

GalaxyTree::GalaxyTree(dvec3* s, int index, const glm::dvec3& c, double w):
	isSubdivised(false),
	solarSystem(s),
	index(index),
	center(c),
	width(w)
{

}

dvec3 cubeDecalage[2][2][2]=
{
	{
		{
			dvec3(-1.0,-1.0,-1.0),
			dvec3(-1.0,-1.0,+1.0),
		},
		{
			dvec3(-1.0,+1.0,-1.0),
			dvec3(-1.0,+1.0,+1.0),
		}
	},
	{
		{
			dvec3(+1.0,-1.0,-1.0),
			dvec3(+1.0,-1.0,+1.0),
		},
		{
			dvec3(+1.0,+1.0,-1.0),
			dvec3(+1.0,+1.0,+1.0),
		}
	},
};

void GalaxyTree::pushSolarSystem(dvec3* s, int ind)
{
	// sauvegarde du solarSystem si il est présent
	dvec3* previousSolarSystem = NULL;
	int previousIndex=0;
	if (not isSubdivised)
	{
		previousSolarSystem = solarSystem;
		previousIndex = index;

		// suppression des systèmes solaires trop proches
		if (glm::distance(*solarSystem,*s) < SOLARSYSTEM_MIN_DISTANCE)
		{
			previousSolarSystem = NULL;
		}

		isSubdivised = true;

		children[0][0][0]=NULL;
		children[0][0][1]=NULL;
		children[0][1][0]=NULL;
		children[0][1][1]=NULL;
		children[1][0][0]=NULL;
		children[1][0][1]=NULL;
		children[1][1][0]=NULL;
		children[1][1][1]=NULL;
	}

	// calcul dans quel branche poser s
	int xx = (s->x>center.x)? 1 : 0;
	int yy = (s->y>center.y)? 1 : 0;
	int zz = (s->z>center.z)? 1 : 0;

	// insertion
	if (children[xx][yy][zz])
	{
		children[xx][yy][zz]->pushSolarSystem(s,ind);
	}else{
		dvec3 c = center + cubeDecalage[xx][yy][zz]*width*0.5;
		children[xx][yy][zz] =
			new GalaxyTree(
					s,
					ind,
					c,
					width*0.5
			);
	}

	
	// réinsertion du précédent solar system si il est là
	if (previousSolarSystem)
		pushSolarSystem(previousSolarSystem,previousIndex);
}

GalaxySolarResponse GalaxyTree::getClosestSolarSystem(const glm::dvec3& pos, double maxDist)
{
	if (isSubdivised)
	{
		dvec3 p = pos - center;
		bool xValid[2], yValid[2], zValid[2];

		xValid[0] = (p.x - maxDist < 0.0);
		xValid[1] = (p.x + maxDist > 0.0);
		yValid[0] = (p.y - maxDist < 0.0);
		yValid[1] = (p.y + maxDist > 0.0);
		zValid[0] = (p.z - maxDist < 0.0);
		zValid[1] = (p.z + maxDist > 0.0);
		list<GalaxySolarResponse> l;
		for(int x=0;x<2;++x)
		for(int y=0;y<2;++y)
		for(int z=0;z<2;++z)
		{
			if (children[x][y][z] and xValid[x] and yValid[y] and zValid[z])
			{
				l.push_back(children[x][y][z]->getClosestSolarSystem(pos,maxDist));
			}
		}
		if (not l.empty())
		{
			l.sort();
			return l.front();
		}
		else
		{
			return {NULL,2*maxDist,-1};
		}
	}
	else
	{
		return {solarSystem,glm::distance(*solarSystem,pos),index};
	}
}


//////////////////////////////////////////////////////////////////////////////

bool GalaxySolarResponse::operator<(const GalaxySolarResponse& other) const
{
	return distance<other.distance;
}
