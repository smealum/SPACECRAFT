#ifndef GALAXY_9PFASIY
#define GALAXY_9PFASIY

#include "utils/glm.h"
#include "render/Camera.h"
#include <vector>

class SolarSystem;
class ShaderProgram;
class ContentHandler;
class Planet;

///////////////////////////////////////////////////////////////
class  Galaxy;
struct GalaxySolarResponse;
class  GalaxyTree;
//////////////////////////////////////////////////////////////


struct GalaxySolarResponse
{
	// position du systeme solaire trouvé
	glm::dvec3* solarSystem;

	// distance depuis la position demandée.
	double distance;

	bool operator<(const GalaxySolarResponse& other) const;
};

//////////////////////////////////////////////////////////////

class Galaxy
{
	public:
		Galaxy();
		~Galaxy();
		Planet* getClosestPlanet(const glm::vec3& pos);
		GalaxySolarResponse getClosestSolarSystem(const glm::dvec3& pos);
		GalaxySolarResponse getClosestSolarSystem(const glm::dvec3& pos, double maxDist);
		void pushSolarSystem(const glm::dvec3& p);

		void step(Camera& camera, ContentHandler& contentHandler,float globalTime);
		void draw(Camera& camera);
	private:
		std::vector<glm::dvec3*> allocatedPositions;

		// OctoTree part
		GalaxyTree* galaxyTree;

		// graphic part
		std::vector<glm::vec3> solarPosition;
		GLuint vao, vbo;
		ShaderProgram& program;
		bool isVBOGenerated;
		void generateVBO();

		//===================
		

		glm::dvec3* selectedPosition;
		SolarSystem* currentSolarSystem;


};

/////////////////////////////////////////////////////////////

class GalaxyTree
{
	public:
		GalaxyTree(glm::dvec3* p, const glm::dvec3& center, double width);
		GalaxySolarResponse getClosestSolarSystem(const glm::dvec3& pos, double maxDist);
		void pushSolarSystem(glm::dvec3* s);

	private:
		bool isSubdivised;
		union
		{
			// Tableau des 8 descendants de l'octree.
			// Il y en a au moins 1 non nul.
			GalaxyTree* children[2][2][2];

			// Le SolarSystem
			glm::dvec3* solarSystem;
		};

		// dimension du noeud
		glm::dvec3 center;
		double width;

};

////////////////////////////////////////////////////////////////


#endif /* end of include guard: GALAXY_9PFASIY */
