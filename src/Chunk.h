#ifndef CHUNK_4TYUGC3N
#define CHUNK_4TYUGC3N

#define BLOCK_SIZE 0.1f
#define CHUNK_N 32
#define CHUNK_SIZE (BLOCK_SIZE*CHUNK_N)

#include "utils/glm.h"
#include "render/Camera.h"
#include "utils/TrackerPointer.h"
#include "Planet.h"

#include <vector>

struct GL_Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec3 normal;
};

//do NOT instanciate directly
//	use new
//do NOT destroy directly
//	use destroyChunk
class Chunk
{
	public:
		Chunk(Planet* p, class MiniWorld* mw, int x, int y, int z, glm::vec3 v1, glm::vec3 v2, glm::vec3 n);

		void draw(Camera& camera, glm::mat4 model);
		void updateData(char data[CHUNK_N][CHUNK_N][CHUNK_N], std::vector<GL_Vertex> va);
		void destroyChunk(void);

		TrackerPointer<Chunk>* getTptr(void);

		Chunk* neighbour[4] ; // NULL <=> nothing
		char value[CHUNK_N][CHUNK_N][CHUNK_N];

	private:
		void initGLObjects();
		void destroyGLObjects();

		int px, py, pz;
		glm::vec3 v1, v2, n;

		TrackerPointer<Chunk>* tptr;
		Planet* planet;
		class MiniWorld* miniWorld;

		GLuint vbo;
		GLuint vao;

		std::vector<GL_Vertex> vArray;
		ShaderProgram &program;
};

#endif /* end of include guard: CHUNK_4TYUGC3N */
