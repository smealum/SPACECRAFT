#ifndef __PLAYERUI_H__
#define __PLAYERUI_H__

#include "utils/glm.h"
#include "render/Shader.h"

class PlayerUI {
	private:
		GLuint vao, vbo, ebo;
		ShaderProgram &program;

	public:
		PlayerUI();
		~PlayerUI();

		void generateVBO();
		void draw();
};

#endif
