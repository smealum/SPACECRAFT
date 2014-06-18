#ifndef __PLAYERUI_H__
#define __PLAYERUI_H__

#include "utils/glm.h"
#include "render/Shader.h"

class PlayerUI {
	private:
		GLuint vao, vbo, ebo;
		ShaderProgram &program;
		float whRatio; // window.width / window.height
		bool valid;

	public:
		PlayerUI();
		~PlayerUI();

		inline void setWhRatio(float v) { whRatio = v; }

		void generateVBO();
		void update(); // when the ratio changes, this must be called, allways after generateVBO
		void draw();
};

#endif
