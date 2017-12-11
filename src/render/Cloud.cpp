#include "Cloud.h"
#include "Application.h"
#include "utils/SphereManager.h"
#include "utils/glm.h"

using namespace std;
using namespace glm;

Cloud::Cloud():
	shader(ShaderProgram::loadFromFile(
				"shader/cloud/cloud.vert",
				"shader/cloud/cloud.frag",
				"cloud"))
{

}

void Cloud::draw(Camera& c)
{
	// update time
    float delta = Application::getInstance().getFrameDeltaTime();
	time+=delta;

	// rotation et animation
	//shader.use();
	////shader.setUniform("time",time);
	//shader.setUniform("model", rotate(mat4(1.0),time*0.01f,vec3(0.0,1.0,0.0)));

	// draw
	//glDisable(GL_CULL_FACE);
    ////glEnable(GL_CULL_FACE);
	//SphereManager::getInstance().draw(c,shader,4);
	//glEnable(GL_CULL_FACE);
}
