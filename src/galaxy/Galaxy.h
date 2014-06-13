#ifndef GALAXY_9PFASIY
#define GALAXY_9PFASIY

#include "utils/glm.h"
#include "render/Camera.h"
#include "SolarSystem.h"

///////////////////////////////////////////////////////////////
class  Galaxy;
struct  GalaxySolarResponse;
class  GalaxyTree;
//////////////////////////////////////////////////////////////


struct GalaxySolarResponse
{
	SolarSystem* solarSystem; // NULL <=> notFound
	double distance;  // distance depuis la position demandée.

	bool operator<(const GalaxySolarResponse& other) const;
};

//////////////////////////////////////////////////////////////

class Galaxy
{
	public:
		Galaxy();
		GalaxySolarResponse getClosestSolarSystem(const glm::dvec3& pos);
		GalaxySolarResponse getClosestSolarSystem(const glm::dvec3& pos, double maxDist);
		void pushSolarSystem(SolarSystem* s);
	private:
		GalaxyTree* galaxyTree;
};

/////////////////////////////////////////////////////////////

class GalaxyTree
{
	public:
		GalaxyTree(SolarSystem* solarSystem, const glm::dvec3& center, double width);
		GalaxySolarResponse getClosestSolarSystem(const glm::dvec3& pos, double maxDist);
		void pushSolarSystem(SolarSystem* s);

	private:
		bool isSubdivised;
		union
		{
			// Tableau des 8 descendants de l'octree.
			// Il y en a au moins 1 non nul.
			GalaxyTree* children[2][2][2];

			// Le SolarSystem
			SolarSystem* solarSystem; // non NULL
		};

		// dimension du noeud
		glm::dvec3 center;
		double width;

};

////////////////////////////////////////////////////////////////


#endif /* end of include guard: GALAXY_9PFASIY */
