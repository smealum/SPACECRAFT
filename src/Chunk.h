#ifndef CHUNK_4TYUGC3N
#define CHUNK_4TYUGC3N

#define CHUNK_SIZE 1.0
#define CHUNK_N 32

#include "utils/glm.h"
#include <vector>
#include "render/Camera.h"
#include <vector>

struct GL_Vertex
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec3 normal;
};

class Chunk
{
    public:
        Chunk* neighbour[4] ; // NULL <=> nothing
        char value[CHUNK_N][CHUNK_N][CHUNK_N];
        void draw(Camera& camera);

        Chunk();
    private:
        void initGLObjects();
        void destroyGLObjects();
        GLuint vbo;
        GLuint vao;

        void computeChunk();

        std::vector<GL_Vertex> vArray;
        ShaderProgram &program;
};

#endif /* end of include guard: CHUNK_4TYUGC3N */
