#ifndef CURSOR_H
#define CURSOR_H

#include "render/Shader.h"
#include "render/Camera.h"
#include "utils/glm.h"
#include "Planet.h"

class Cursor
{
	public:
		Cursor();

		void draw(Camera& c);

		void setPosition(glm::i32vec3 pos, glm::vec3 origin, glm::vec3 v1, glm::vec3 v2);
		void unaffect(void);

	private:
		bool affected;
		glm::i32vec3 pos;
		glm::vec3 origin, v1, v2;
		PlanetFace* toplevel;

		ShaderProgram& shader;
		GLuint vao, vbo;
};

#endif
