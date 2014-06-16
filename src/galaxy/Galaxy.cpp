#include "Galaxy.h"
#include <list>
#include "utils/dbg.h"
#include "SolarSystem.h"
#include "render/Shader.h"


using namespace glm;
using namespace std;

/////////////////////////////////////////////////////////
// paramètres de la galaxie
const double GALAXY_WIDTH = 2e10;
const dvec3 GALAXY_CENTER(0.0,0.0,0.0);
const double SOLARSYSTEM_MIN_DISTANCE = 0.001; 

/////////////////////////////////////////////////////////

Galaxy::Galaxy():
	galaxyTree(NULL),
	vbo(0),
	vao(0),
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
	solarPosition.push_back(vec3(s));

	dvec3* solarSystem = new dvec3(s);
	allocatedPositions.push_back(solarSystem);

	if (galaxyTree)
	{
		galaxyTree->pushSolarSystem(solarSystem);
	}
	else
	{
		galaxyTree = new GalaxyTree(solarSystem,GALAXY_CENTER, GALAXY_WIDTH);
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
	return {NULL,2.0*maxDist};
}

void Galaxy::generateVBO()
{
	isVBOGenerated = true;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*solarPosition.size(), &solarPosition[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindFragDataLocation(program.getHandle(), 0, "outColor");
	program.setAttribute("position", 3, GL_FALSE, 0, 0);

	program.setBuffers(vao, vbo, 0);
	program.use();

}


void Galaxy::step(Camera& camera, ContentHandler& contentHandler)
{
	
	dvec3 origin(0.0,0.0,0.0);
	dvec3 position = camera.getPositionDouble(origin);
	if(selectedPosition)position+=*selectedPosition;
	GalaxySolarResponse r = getClosestSolarSystem(position,1e9);

	// log_info("%f",r.distance);
	// return;

	// printf("DIST %f\n",glm::length(camera.getPositionDouble(origin)));

	if (r.solarSystem and r.distance<1e4)
	{
		dvec3 p = *(r.solarSystem);
		// log_info("Plus proche : (%f,%f,%f) = %f", p.x, p.y, p.z, r.distance);

		if (selectedPosition == r.solarSystem) return;

		log_info("%f",r.distance);
		printf("NEW NEW NEW %p\n",selectedPosition);

		if(selectedPosition)camera.movePositionDouble(*selectedPosition);

		// suppression du précédent système solaire.
		if (currentSolarSystem)
		{
			// TODO suppression du contenu généré
			currentSolarSystem = NULL;
		}

		// ajout du nouveau système solaire.
		selectedPosition = r.solarSystem;
		// currentSolarSystem = new SolarSystem(p);
		// currentSolarSystem->generate(contentHandler);

		camera.movePositionDouble(-*selectedPosition);

		return;
	}else{
		// suppression du précédent système solaire.
		// log_info("Trop loin");
		if(selectedPosition)camera.movePositionDouble(*selectedPosition);
		selectedPosition = NULL;
		currentSolarSystem = NULL;
	}
}


void Galaxy::draw(Camera& camera)
{
	if (not isVBOGenerated) generateVBO();
    program.use();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    camera.updateCamera(program);

    if(selectedPosition)program.setUniform("model",glm::translate(glm::mat4(1.0f),glm::vec3(-*selectedPosition)-camera.getReference()));
    else program.setUniform("model",glm::translate(glm::mat4(1.0f),-camera.getReference()));

    glDrawArrays(GL_POINTS, 0, solarPosition.size());
	
	// TODO intégration
	return;
	
	// affichage du système solaire le plus proche
	if (currentSolarSystem)
	{
		currentSolarSystem->draw(camera);
	}
}

////////////////////////////////////////////////////////

GalaxyTree::GalaxyTree(dvec3* s, const glm::dvec3& c, double w):
	isSubdivised(false),
	solarSystem(s),
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

void GalaxyTree::pushSolarSystem(dvec3* s)
{
	// sauvegarde du solarSystem si il est présent
	dvec3* previousSolarSystem = NULL;
	if (not isSubdivised)
	{
		previousSolarSystem = solarSystem;

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
		children[xx][yy][zz]->pushSolarSystem(s);
	}
	else
	{
		dvec3 c = center + cubeDecalage[xx][yy][zz]*width*0.5;
		children[xx][yy][zz] =
			new GalaxyTree(
					s,
					c,
					width*0.5
			);
	}

	
	// réinsertion du précédent solar system si il est là
	if (previousSolarSystem)
		pushSolarSystem(previousSolarSystem);
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
			return {NULL,2*maxDist};
		}
	}
	else
	{
		return {solarSystem,glm::distance(*solarSystem,pos)};
	}
}


//////////////////////////////////////////////////////////////////////////////

bool GalaxySolarResponse::operator<(const GalaxySolarResponse& other) const
{
	return distance<other.distance;
}
