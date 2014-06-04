#ifndef SPHERE_H
#define SPHERE_H

#include <list>
#include <vector>
#include "utils/glm.h"
#include "render/Camera.h"

class Sphere
{
	public:
		Sphere(ShaderProgram& shader, int lod);
		
		void subDivide(int d);
		void generateVBO(void);
		void draw(Camera& c);

	private:
		ShaderProgram& shader;
		int lod;

		GLuint vbo, ebo, vao;

		std::vector<glm::vec3> vertices;
		std::list<glm::i32vec3> faces;
};

#endif
