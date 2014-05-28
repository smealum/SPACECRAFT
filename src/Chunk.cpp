#include "Chunk.h"

using namespace std;
using namespace glm;

Chunk::Chunk():
    program(ShaderProgram::loadFromFile("shader/basic/basic.vert", "shader/basic/basic.frag", "basic"))
{
    for(int x=0;x<CHUNK_N;x++)
    {
        for(int y=0;y<CHUNK_N;y++)
        {
            for(int z=0;z<CHUNK_N;z++)
            {
                value[x][y][z]=x+y*x+z*x*z;
            }
        }
    }

    initGLObjects();
}

void Chunk::computeChunk()
{
    vArray.clear();
    // X
    for(int y=1;y<CHUNK_N;++y)
    for(int z=1;z<CHUNK_N;++z)
    for(int x=1;x<CHUNK_N;++x)
    {
        if (value[x][y][z])
        {
            if (!value[x][y][z-1])
            {
                GL_Vertex v1=
                {
                   vec3(x,y,z),
                   vec4(value[x][y][z]),
                   vec3(-1,0,0)
                };
                vArray.push_back(v1);
                GL_Vertex v2=
                {
                   vec3(x,y,z+1),
                   vec4(value[x][y][z]),
                   vec3(-1,0,0)
                };
                vArray.push_back(v2);
                GL_Vertex v3=
                {
                   vec3(x,y+1,z),
                   vec4(value[x][y][z]),
                   vec3(-1,0,0)
                };
                vArray.push_back(v3);
                GL_Vertex v4=
                {
                   vec3(x,y,z),
                   vec4(value[x][y][z+1]),
                   vec3(-1,0,0)
                };
                vArray.push_back(v4);
                GL_Vertex v5=
                {
                   vec3(x,y,z+1),
                   vec4(value[x][y+1][z+1]),
                   vec3(-1,0,0)
                };
                vArray.push_back(v5);
                GL_Vertex v6=
                {
                   vec3(x,y+1,z+1),
                   vec4(value[x][y+1][z]),
                   vec3(-1,0,0)
                };
                vArray.push_back(v6);
            }
        }
        else
        {
            if (value[x][y][z-1])
            {
                GL_Vertex v1=
                {
                   vec3(x,y,z),
                   vec4(value[x-1][y][z]),
                   vec3(1,0,0)
                };
                vArray.push_back(v1);
                GL_Vertex v2=
                {
                   vec3(x,y+1,z),
                   vec4(value[x-1][y][z]),
                   vec3(1,0,0)
                };
                vArray.push_back(v2);
                GL_Vertex v3=
                {
                   vec3(x,y,z+1),
                   vec4(value[x-1][y][z]),
                   vec3(1,0,0)
                };
                vArray.push_back(v3);
                GL_Vertex v4=
                {
                   vec3(x,y,z),
                   vec4(value[x-1][y][z+1]),
                   vec3(1,0,0)
                };
                vArray.push_back(v4);
                GL_Vertex v5=
                {
                   vec3(x,y+1,z+1),
                   vec4(value[x-1][y+1][z+1]),
                   vec3(1,0,0)
                };
                vArray.push_back(v5);
                GL_Vertex v6=
                {
                   vec3(x,y,z+1),
                   vec4(value[x-1][y+1][z]),
                   vec3(1,0,0)
                };
                vArray.push_back(v6);
            }
        }
    }
}

void Chunk::draw(Camera& cam)
{
    program.use();
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    program.setUniform("projection",cam.proj);
    program.setUniform("model",mat4(1.0));
    program.setUniform("view",cam.view);
    glDrawArrays(GL_TRIANGLES, 0 ,  vArray.size()); 
}

void Chunk::initGLObjects()
{
    computeChunk();

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_Vertex)*vArray.size(), &(vArray[0]), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    program.setBuffers(vao, vbo, 0);
    program.use();
    glBindFragDataLocation(program.getHandle(), 0, "outColor");
}
void Chunk::destroyGLObjects()
{

}
