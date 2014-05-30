#include "Chunk.h"
#include "data/ContentHandler.h"
#include "utils/dbg.h"

using namespace std;
using namespace glm;

Chunk::Chunk(Planet* p, class MiniWorld* mw, int x, int y, int z, glm::vec3 v1, glm::vec3 v2, glm::vec3 n):
    program(ShaderProgram::loadFromFile("shader/chunk/chunk.vert", "shader/chunk/chunk.frag", "chunk")),
    tptr(new TrackerPointer<Chunk>(this, true)),
    planet(p),
    miniWorld(mw),
    px(x),
    py(y),
    pz(z),
    v1(v1),
    v2(v2),
    n(n)
{
    planet->handler.requestContent(new WorldChunkRequest(*planet, *this, glm::vec3(0.f,0.f,0.f), glm::vec3(1.f,1.f,1.f), x, y, z));

    memset(value,0,sizeof(char)*CHUNK_N*CHUNK_N*CHUNK_N);

    initGLObjects();
}

void Chunk::updateData(char data[CHUNK_N][CHUNK_N][CHUNK_N], std::vector<GL_Vertex> va)
{
    memcpy(value,data,sizeof(char)*CHUNK_N*CHUNK_N*CHUNK_N);
    vArray.swap(va);
    destroyGLObjects();
    initGLObjects();
}

void Chunk::destroyChunk(void)
{
    tptr->release();
}

void Chunk::draw(Camera& cam, glm::mat4 model)
{
    program.use();
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    program.setUniform("projection",cam.proj);
    
    glm::mat3 modeltest=glm::mat3(v1,n*length(v1),v2);

    program.setUniform("model",glm::translate(model*glm::mat4(modeltest), glm::vec3(px,py,pz)*CHUNK_SIZE));
    program.setUniform("view",cam.view);
    glDrawArrays(GL_TRIANGLES, 0 ,  vArray.size()); 
}

TrackerPointer<Chunk>* Chunk::getTptr(void)
{
    return tptr;
}

void Chunk::initGLObjects()
{
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_Vertex)*vArray.size(), &(vArray[0]), GL_STATIC_DRAW);

    // log_info("%d octet",sizeof(GL_Vertex) * vArray.size());
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    program.setBuffers(vao, vbo, 0);
    program.use();
    glBindFragDataLocation(program.getHandle(), 0, "outColor");
    program.setAttribute("position", 3, GL_FALSE, 10, 0);
    program.setAttribute("color", 4, GL_FALSE, 10, 3);
    program.setAttribute("normal", 3, GL_FALSE, 10, 7);
}

void Chunk::destroyGLObjects()
{
    //TODO !
}
