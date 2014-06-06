#include "Sun.h"
#include "Application.h"
#include "utils/SphereManager.h"
#include "utils/glm.h"
#include "utils/dbg.h"

#define SunWidth (108.0f)

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
	position(8.0,0.0,0.0),
	time(0.0)
{

}

void Sun::draw(Camera& c)
{
	// mise a jour du levelOfDetails
	glm::vec3 p=c.getPosition();
	
	double l = length(p-position)/SunWidth;
	lod=clamp(400/log(l),3.0,4.0);

	// update time
    float delta = Application::getInstance().getFrameDeltaTime();
	time+=delta;

	log_info("time=%f",time);

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


}
void Sun::drawGlow(Camera& c)
{
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

void Sun::setPosition(glm::vec3 p)
{
	position = p;
}
