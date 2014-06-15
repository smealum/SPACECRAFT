#ifndef GALAXY_9PFASIY
#define GALAXY_9PFASIY

#include "utils/glm.h"
#include "render/Camera.h"
#include <vector>

class SolarSystem;
class ShaderProgram;
class ContentHandler;

///////////////////////////////////////////////////////////////
class  Galaxy;
struct GalaxySolarResponse;
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

		void step(Camera& camera, ContentHandler& contentHandler);
		void draw(Camera& camera);
	private:

		// OctoTree part
		GalaxyTree* galaxyTree;

		// graphic part
		std::vector<glm::vec3> solarPosition;
		GLuint vao, vbo;
		ShaderProgram& program;
		bool isVBOGenerated;
		void generateVBO();

		//===================
		
		SolarSystem* selectedSolarSystem;
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
