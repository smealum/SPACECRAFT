#include "SphereManager.h"
#include "utils/dbg.h"

using namespace std;

void SphereManager::draw(Camera& c,ShaderProgram& shader, int levelOfDetails)
{


	// recherche d'une sphere de la bonne taille
	auto it1 = spheres.find(shader.getHandle());
	if (it1 != spheres.end())
	{
		auto it2 = it1->second.find(levelOfDetails);
		if (it2 != it1->second.end())
		{
			it2->second->draw(c);
			return;
		}
	}

	log_info("Sphere gen: Shd=%s lod=%d",shader.getName().c_str(),levelOfDetails);

	// pas trouvé, on le génère.
	Sphere* sphere = new Sphere(shader,levelOfDetails);
	sphere->generateVBO();
	sphere->draw(c);

	// on l'enregistre dans la map
	spheres[shader.getHandle()][levelOfDetails] = sphere;
}
