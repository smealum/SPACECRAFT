#include "Sun.h"
#include "Application.h"
#include "utils/SphereManager.h"
#include "utils/glm.h"

using namespace std;
using namespace glm;

Sun::Sun():
	shader(ShaderProgram::loadFromFile(
				"shader/sun/sun.vert",
				"shader/sun/sun.frag",
				"sun")),
	shaderGlow(ShaderProgram::loadFromFile(
				"shader/sun/sunGlow.vert",
				"shader/sun/sunGlow.frag",
				"sunGlow")),
	position(8.0,0.0,0.0)
{

}

void Sun::draw(Camera& c)
{
	glm::vec3 p=c.getPosition();

	
	double l = length(p-position);
	int lod=clamp(4.0/log(l),3.0,4.0);


	// update time
    float delta = Application::getInstance().getFrameDeltaTime();
	time+=delta;

	// sun inner
		// rotation et animation
		shader.use();
		shader.setUniform("time",time);
		shader.setUniform("model",
				translate(
					mat4(1.0),
					position
				)
		);

		// draw
		SphereManager::getInstance().draw(c,shader,lod);
		//glEnable(GL_CULL_FACE);


	// sun glow
		// rotation et animation
		shaderGlow.use();
		shaderGlow.setUniform("time",time);
		shaderGlow.setUniform("model",
				translate(
					mat4(1.0),
					position
				)
		);

		// draw
		glFrontFace(GL_CW);
		SphereManager::getInstance().draw(c,shaderGlow,lod-1);
		glFrontFace(GL_CCW);
}