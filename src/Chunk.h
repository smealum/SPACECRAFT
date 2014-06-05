#ifndef CHUNK_4TYUGC3N
#define CHUNK_4TYUGC3N

#define BLOCK_SIZE 1.0f
#define CHUNK_N 32
#define CHUNK_SIZE (BLOCK_SIZE*CHUNK_N)

#include "utils/glm.h"
#include "render/Camera.h"
#include "utils/TrackerPointer.h"
#include "Planet.h"

#include <vector>
typedef uint8_t chunkVal;

struct GL_Vertex
{
	glm::vec3 position;
	glm::vec2 texcoord;
	float facedir;
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
		void updateData(chunkVal* data, std::vector<GL_Vertex> va);
		void destroyChunk(void);

		glm::i32vec3 performRayMarch(glm::dvec3 localBlockPosf, glm::dvec3 localBlockPosf2, int* dir);
		bool collidePoint(glm::dvec3& p, glm::dvec3& v);
		bool selectBlock(glm::dvec3 p, glm::dvec3 v, glm::i32vec3& out);

		TrackerPointer<Chunk>* getTptr(void);

		Chunk* neighbour[4] ; // NULL <=> nothing
		chunkVal value[CHUNK_N+2][CHUNK_N+2][CHUNK_N+2];

	private:
		void initGLObjects();
		void destroyGLObjects();

		int px, py, pz;
		glm::vec3 v1, v2, origin, n;
		glm::vec3 boundingVolume[8];

		TrackerPointer<Chunk>* tptr;
		Planet* planet;
		class MiniWorld* miniWorld;

		GLuint vbo;
		GLuint vao;

		std::vector<GL_Vertex> vArray;
		ShaderProgram &program;
};

#endif /* end of include guard: CHUNK_4TYUGC3N */
