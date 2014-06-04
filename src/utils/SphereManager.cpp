#include "SphereManager.h"

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

	// pas trouvé, on le génère.
	Sphere* sphere = new Sphere(shader,levelOfDetails);
	spheres[shader.getHandle()][levelOfDetails] = sphere;
	sphere->draw(c);
}
