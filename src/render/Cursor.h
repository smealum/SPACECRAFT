#ifndef CURSOR_H
#define CURSOR_H

#include "render/Shader.h"
#include "render/Camera.h"
#include "utils/glm.h"
#include "Planet.h"

#define PI 3.14159265f

class Cursor
{
	public:
		Cursor();

		void draw(Camera& c);

		void setPosition(glm::i32vec3 pos, int dir, glm::vec3 origin, glm::vec3 v1, glm::vec3 v2, glm::mat4 planetModel, int numBlocks);
		void unaffect(void);

	private:
		bool affected;

		int dir;
		glm::i32vec3 pos;

		glm::mat4 planetModel;
		glm::vec3 origin, v1, v2;
		PlanetFace* toplevel;
		int numBlocks;

		ShaderProgram& shader;
		GLuint vao, vbo;
};

#endif
