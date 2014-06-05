#include "render/Cursor.h"

static GLfloat vertices[] =
{
	-1.0f, -1.0f, 0.0f,
	-1.0f, +1.0f, 0.0f,
	+1.0f, +1.0f, 0.0f,
	+1.0f, -1.0f, 0.0f,
};

Cursor::Cursor():
	affected(false),
	shader(ShaderProgram::loadFromFile("shader/cursor/cursor.vert", "shader/cursor/cursor.frag", "cursor"))
{
	//generate VBO
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	shader.setBuffers(vao, vbo, 0);

	shader.use();
	glBindFragDataLocation(shader.getHandle(), 0, "outColor");
	shader.setAttribute("position", 3, GL_FALSE, 3, 0);
}

void Cursor::draw(Camera& c)
{
	if(!affected)return;

	shader.use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	c.updateCamera(shader);

	shader.setUniform("model", glm::mat4(1.0f));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 12);
}

void Cursor::setPosition(glm::i32vec3 pos, glm::vec3 origin, glm::vec3 v1, glm::vec3 v2)
{
	affected=true;
	this->pos=pos;
	this->origin=origin;
	this->v1=v1;
	this->v2=v2;
}

void Cursor::unaffect(void)
{
	affected=false;
}
