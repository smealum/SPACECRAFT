#include "Galaxy.h"
#include <list>

using namespace glm;
using namespace std;

const double GALAXY_WIDTH = 100.0;
const dvec3 GALAXY_CENTER(0.0,0.0,0.0);

// TODO penser a vérifier que 2 solar systèmes ne possèdent pas la même position;

/////////////////////////////////////////////////////////

Galaxy::Galaxy():
	galaxyTree(NULL)
{
	
}

void Galaxy::pushSolarSystem(SolarSystem* s)
{
	if (galaxyTree)
	{
		galaxyTree->pushSolarSystem(s);
	}
	else
	{
		galaxyTree = new GalaxyTree(s,GALAXY_CENTER, GALAXY_WIDTH);
	}
}


GalaxySolarResponse Galaxy::getClosestSolarSystem(const dvec3& pos , double maxDist)
{
	if (galaxyTree)
		return galaxyTree->getClosestSolarSystem(pos,maxDist);
	else
		return {NULL,maxDist};
}

////////////////////////////////////////////////////////

GalaxyTree::GalaxyTree(SolarSystem* s, const glm::dvec3& c, double w):
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
			dvec3(+1.0,-1.0,-1.0),
		},
		{
			dvec3(-1.0,+1.0,-1.0),
			dvec3(+1.0,+1.0,-1.0),
		}
	},
	{
		{
			dvec3(-1.0,-1.0,+1.0),
			dvec3(+1.0,-1.0,+1.0),
		},
		{
			dvec3(-1.0,+1.0,+1.0),
			dvec3(+1.0,+1.0,+1.0),
		}
	},
};

void GalaxyTree::pushSolarSystem(SolarSystem* s)
{
	// sauvegarde du solarSystem si il est présent
	SolarSystem* previousSolarSystem = NULL;
	if (not isSubdivised)
	{
		previousSolarSystem = solarSystem;
		isSubdivised = true;
		children={{{NULL,NULL},{NULL,NULL}},{{NULL,NULL},{NULL,NULL}}};
	}

	// calcul dans quel branche poser s
	const dvec3 sPos = s->getPosition();
	int xx = (sPos.x>center.x)? 1 : 0;
	int yy = (sPos.y>center.y)? 1 : 0;
	int zz = (sPos.z>center.z)? 1 : 0;

	// insertion
	if (children[xx][yy][zz])
	{
		children[xx][yy][zz]->pushSolarSystem(s);
	}
	else
	{
		children[xx][yy][zz] =
			new GalaxyTree(
					s,
					cubeDecalage[xx][yy][zz]*width*0.5,
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

		xValid[0] = (p.x + maxDist > 0.0);
		xValid[1] = (p.x - maxDist > 0.0);
		yValid[0] = (p.y + maxDist > 0.0);
		yValid[1] = (p.y - maxDist > 0.0);
		zValid[0] = (p.z + maxDist > 0.0);
		zValid[1] = (p.z - maxDist > 0.0);
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
		l.sort();
		return l.front(); // TODO protection (assert)
	}
	else
	{
		return {solarSystem,distance(solarSystem->getPosition(),pos)};
	}
}


//////////////////////////////////////////////////////////////////////////////

bool GalaxySolarResponse::operator<(const GalaxySolarResponse& other) const
{
	return distance<other.distance;
}

