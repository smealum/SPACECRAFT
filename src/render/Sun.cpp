#include <iostream>
#include "Sun.h"
#include "Application.h"
#include "utils/SphereManager.h"
#include "utils/glm.h"

using namespace std;
using namespace glm;

Sun::Sun(vec3 position, float size, float color):
	shader(ShaderProgram::loadFromFile(
				"shader/sun/sun.vert",
				"shader/sun/sun.frag",
				"sun")),
	shaderGlow(ShaderProgram::loadFromFile(
				"shader/sun/sunGlow.vert",
				"shader/sun/sunGlow.frag",
				"sunGlow")),
	position(position),
	size(size),
	time(0.0),
	spectrum(color)
{
	primaryColor=glm::rgbColor(glm::vec3(10.0,1.0,1.0)*(1-spectrum)+glm::vec3(50.0,0.0,1.0)*(spectrum));
	secondaryColor=glm::rgbColor(glm::vec3(30.0,1.0,1.0)*(1-spectrum)+glm::vec3(70.0,0.5,1.0)*(spectrum));
}

void Sun::draw(Camera& c)
{
	// mise a jour du levelOfDetails
	glm::vec3 p=c.getPosition(position);
	
	double l = length(p)/size;
	lod=clamp(4.f/log(glm::max(l,1.1)),3.0,4.0);

	// update time
    float delta = Application::getInstance().getFrameDeltaTime();
	time+=delta;

	// rotation et animation
	shader.use();
	shader.setUniform("time",time);
	shader.setUniform("primaryColor",primaryColor);
	shader.setUniform("secondaryColor",secondaryColor);
	shader.setUniform("model",
			translate(
				mat4(1.0f),
				position-c.getReference()
			)*mat4(mat3(size))
	);

	// draw
	SphereManager::getInstance().draw(c,shader,lod);
	//glEnable(GL_CULL_FACE);
}

void Sun::drawGlow(Camera& c)
{
	// rotation et animation
	shaderGlow.use();
	shaderGlow.setUniform("time",time);
	shaderGlow.setUniform("model",
			translate(
				mat4(1.0f),
				position-c.getReference()
			)*mat4(mat3(size))
	);

	// draw
	glFrontFace(GL_CW);
	SphereManager::getInstance().draw(c,shaderGlow,lod);
	glFrontFace(GL_CCW);
}

void Sun::setPosition(glm::vec3 p)
{
	position = p;
}

glm::vec3 Sun::getPosition(void)
{
	return position;
}
