#include "utils/sphere.h"
#include <vector>
#include <algorithm>

const glm::vec3 originalVertices[]=
{
	glm::vec3(-1.0f,0.0f,0.0f),
	glm::vec3(1.0f,0.0f,0.0f),
	glm::vec3(0.0f,-1.0f,0.0f),
	glm::vec3(0.0f,1.0f,0.0f),
	glm::vec3(0.0f,0.0f,-1.0f),
	glm::vec3(0.0f,0.0f,1.0f)
};
int numOriginalVertices=6;

const glm::i32vec3 originalFaces[]=
{
	glm::i32vec3(0,4,2),
	glm::i32vec3(2,5,0),
	glm::i32vec3(1,5,2),
	glm::i32vec3(4,1,2),
	glm::i32vec3(3,1,4),
	glm::i32vec3(3,4,0),
	glm::i32vec3(0,5,3),
	glm::i32vec3(5,1,3)
};
int numOriginalFaces=8;

Sphere::Sphere(ShaderProgram& shader, int lod):
	shader(shader),
	lod(lod)
{
	for(int i=0;i<numOriginalVertices;i++)vertices.push_back(originalVertices[i]);
	for(int i=0;i<numOriginalFaces;i++)faces.push_back(originalFaces[i]);

	subDivide(lod);
}

void Sphere::subDivide(int d)
{
	if(d<=0)return;

	int l=faces.size();

	while(l>0)
	{
		glm::i32vec3 f=faces.front();
		faces.pop_front();

		int id=vertices.size();
		vertices.push_back(glm::normalize((vertices[f.x]+vertices[f.y])*0.5f));
		vertices.push_back(glm::normalize((vertices[f.x]+vertices[f.z])*0.5f));
		vertices.push_back(glm::normalize((vertices[f.z]+vertices[f.y])*0.5f));

		faces.push_back(glm::i32vec3(id+2,id+1,id));
		faces.push_back(glm::i32vec3(f.x,id,id+1));
		faces.push_back(glm::i32vec3(id+2,id,f.y));
		faces.push_back(glm::i32vec3(f.z,id+1,id+2));

		l--;
	}

	subDivide(d-1);
}

void Sphere::generateVBO(void)
{
	std::vector<float> vertexVect;
	std::vector<int> faceVect;

	for(auto it=vertices.begin(); it!=vertices.end(); ++it)
	{
		vertexVect.push_back(it->x);
		vertexVect.push_back(it->y);
		vertexVect.push_back(it->z);
	}

	for(auto it=faces.begin(); it!=faces.end(); ++it)
	{
		faceVect.push_back(it->x);
		faceVect.push_back(it->y);
		faceVect.push_back(it->z);
	}

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexVect.size()*4, &vertexVect[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faceVect.size()*4, &faceVect[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	shader.setBuffers(vao, vbo, ebo);
}

void Sphere::draw(Camera& c)
{
	shader.use();

	shader.setUniform("model", glm::mat4(1.f));

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	c.updateCamera(shader);

	glDrawElements(GL_TRIANGLES, faces.size()*3, GL_UNSIGNED_INT, 0);
}
