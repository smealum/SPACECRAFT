#ifndef CHUNK_4TYUGC3N
#define CHUNK_4TYUGC3N

#define CHUNK_SIZE 0.1f
#define CHUNK_N 32

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

//do NOT destroy directly
//use destroyChunk
class Chunk
{
    public:
        Chunk(Planet* p);

        void draw(Camera& camera);
        void updateData(char data[CHUNK_N][CHUNK_N][CHUNK_N]);
        void destroyChunk(void);
        
        TrackerPointer<Chunk>* getTptr(void);

        Chunk* neighbour[4] ; // NULL <=> nothing
        char value[CHUNK_N][CHUNK_N][CHUNK_N];

    private:
        void initGLObjects();
        void destroyGLObjects();
        
        void computeChunk();

        TrackerPointer<Chunk>* tptr;
        Planet* planet;

        GLuint vbo;
        GLuint vao;

        std::vector<GL_Vertex> vArray;
        ShaderProgram &program;
};

#endif /* end of include guard: CHUNK_4TYUGC3N */
