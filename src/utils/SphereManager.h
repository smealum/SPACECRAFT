#ifndef SPHEREMANAGER_PUOOHC77
#define SPHEREMANAGER_PUOOHC77

#include "Sphere.h"
#include "render/Shader.h"
#include "utils/Singleton.h"
#include <map>

class SphereManager : public Singleton<SphereManager>
{
	public:
		void draw(Camera& c,ShaderProgram& shader, int levelOfDetails);
	private:
		std::map<GLuint,std::map<int,Sphere*> > spheres;
    	friend class Singleton<SphereManager>;
};


#endif /* end of include guard: SPHEREMANAGER_PUOOHC77 */


