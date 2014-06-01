#include "Chunk.h"
#include "MiniWorld.h"
#include "data/ContentHandler.h"
#include "utils/dbg.h"

using namespace std;
using namespace glm;

Chunk::Chunk(Planet* p, class MiniWorld* mw, int x, int y, int z, glm::vec3 v1, glm::vec3 v2, glm::vec3 origin):
    program(ShaderProgram::loadFromFile("shader/chunk/chunk.vert", "shader/chunk/chunk.frag", "shader/chunk/chunk.geom", "chunk")),
    tptr(new TrackerPointer<Chunk>(this, true)),
    planet(p),
    miniWorld(mw),
    px(x),
    py(y),
    pz(z),
    v1(v1),
    v2(v2),
    origin(origin)
{
    planet->handler.requestContent(new WorldChunkRequest(*planet, *this, mw->face->elevation, origin, v1, v2, x, y, z));

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
    if(!vArray.size())return;
 
    program.use();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    cam.updateCamera(program);

    program.setUniform("origin",origin);
    program.setUniform("v1",v1);
    program.setUniform("v2",v2);
    program.setUniform("numBlocks",float(PLANETFACE_BLOCKS));

    glDrawArrays(GL_POINTS, 0 ,  vArray.size()); 
}

TrackerPointer<Chunk>* Chunk::getTptr(void)
{
    return tptr;
}

void Chunk::initGLObjects()
{
    if(vArray.size())
    {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GL_Vertex)*vArray.size(), &(vArray[0]), GL_STATIC_DRAW);

        // log_info("%d octet", sizeof(GL_Vertex) * vArray.size());
        // log_info("%d faces", vArray.size());
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        program.setBuffers(vao, vbo, 0);
        program.use();
        glBindFragDataLocation(program.getHandle(), 0, "outColor");
        program.setAttribute("position", 3, GL_FALSE, 4, 0);
        program.setAttribute("facedir", 3, GL_FALSE, 4, 3);
    }else{
        vbo=vao=0;
    }
}

void Chunk::destroyGLObjects()
{
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteBuffers(1, &vao);
    vbo=vao=0;
}
