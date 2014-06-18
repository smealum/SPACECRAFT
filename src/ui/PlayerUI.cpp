#include "ui/PlayerUI.h"
#include "world/BlockType.h"
#include "utils/gldbg.h"

extern uint8_t selectBlockType;

static GLfloat vertices[] = {
	-0.5f,  -0.5f, 0.0f, 1.0f, // bot-left
	+0.5f,  -0.5f, 1.0f, 1.0f,  // bot-right
	+0.5f, +0.5f, 1.0f, 0.0f,  // top-right
	-0.5f, +0.5f, 0.0f, 0.0f // top-left
};

static GLuint elements[] = {
	0,1,2,
	0,2,3
};

PlayerUI::PlayerUI() :
	program(ShaderProgram::loadFromFile("shader/ui/player.vert",
				"shader/ui/player.frag",
				"ui")),
	whRatio(1.f),
	valid(false)
{
}

PlayerUI::~PlayerUI()
{
	if (valid)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}
}

void PlayerUI::generateVBO()
{
	if (valid)
		return;
	valid = true;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5*4, vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6, elements, GL_STATIC_DRAW);

	glBindFragDataLocation(program.getHandle(), 0, "outColor");
	program.setAttribute("position", 2, GL_FALSE, 4, 0);
	program.setAttribute("texcoord", 2, GL_FALSE, 4, 2);

	program.setBuffers(vao, vbo, ebo);
	program.use();
}

void PlayerUI::draw()
{
	program.use();
	program.setUniform("tile", selectBlockType-1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void PlayerUI::update()
{
	float ts = 0.075f; // width heigth of the tile preview
	float toffset = 0.05f; // offset from bot and right

	// height values must be adapted

	// botleft, botright, topright, topleft
	vertices[0*4 + 0] = +1.f - (toffset + ts);
	vertices[0*4 + 1] = -1.f + (toffset) * whRatio;

	vertices[1*4 + 0] = +1.f - (toffset);
	vertices[1*4 + 1] = -1.f + (toffset) * whRatio;

	vertices[2*4 + 0] = +1.f - (toffset);
	vertices[2*4 + 1] = -1.f + (toffset + ts) * whRatio;

	vertices[3*4 + 0] = +1.f - (toffset + ts);
	vertices[3*4 + 1] = -1.f + (toffset + ts) * whRatio;

	if (valid)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5*4, vertices, GL_STATIC_DRAW);
	}

}

