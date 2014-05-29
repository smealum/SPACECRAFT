#include "Chunk.h"
#include "data/ContentHandler.h"
#include "utils/dbg.h"

using namespace std;
using namespace glm;

Chunk::Chunk(Planet* p):
    program(ShaderProgram::loadFromFile("shader/phong/phong.vert", "shader/phong/phong.frag", "phong")),
    tptr(new TrackerPointer<Chunk>(this, true)),
    planet(p)
{
    // for(int x=0;x<CHUNK_N;x++)
    // {
    //     for(int y=0;y<CHUNK_N;y++)
    //     {
    //         for(int z=0;z<CHUNK_N;z++)
    //         {
				// //if (
				// 		//(x-CHUNK_N/2)*(x-CHUNK_N/2)+
				// 		//(y-CHUNK_N/2)*(y-CHUNK_N/2)+
				// 		//(z-CHUNK_N/2)*(z-CHUNK_N/2)

				// 		//< CHUNK_N*CHUNK_N/4

				// 		//and y<CHUNK_N/2
				// 	//)
				// if ( (x+y+z +x*y*z) % 20 == 0)
    //             {
    //                 value[x][y][z]=x+y*x+z*x*z;
    //                 if (value[x][y][z]==0)
    //                     value[x][y][z]=1;
    //             }
    //             else
    //             {
    //                 value[x][y][z]=0;
    //             }
    //         }
    //     }
    // }

    planet->handler.requestContent(new WorldChunkRequest(*planet, *this, glm::vec3(0.f,0.f,0.f), glm::vec3(1.f,1.f,1.f)));

    memset(value,0,sizeof(char)*CHUNK_N*CHUNK_N*CHUNK_N);

    initGLObjects();
}

void Chunk::updateData(char data[CHUNK_N][CHUNK_N][CHUNK_N])
{
    memcpy(value,data,sizeof(char)*CHUNK_N*CHUNK_N*CHUNK_N);
    destroyGLObjects();
    initGLObjects();
}

void Chunk::destroyChunk(void)
{
    tptr->release();
}

void Chunk::computeChunk()
{
    vArray.clear();
    // X
    for(int y=0;y<CHUNK_N;++y)
    for(int z=0;z<CHUNK_N;++z)
    for(int x=1;x<CHUNK_N;++x)
    {
        if (value[x][y][z])
        {
            if (!value[x-1][y][z])
            {
                GL_Vertex v;
                v.color=vec4(
                        double(x)/double(CHUNK_N),
                        double(y)/double(CHUNK_N),
                        double(z)/double(CHUNK_N),
                        1.0
                );
                v.normal=vec3(-1,0,0);
                v.position=vec3(x,y,z)     *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y,z+1)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y,z+1)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z+1) *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z)  *CHUNK_SIZE; vArray.push_back(v);
            }
        }
        else
        {
            if (value[x-1][y][z])
            {
                GL_Vertex v;
                v.color=vec4(
                        double(x-1)/double(CHUNK_N),
                        double(y)/double(CHUNK_N),
                        double(z)/double(CHUNK_N),
                        1.0
                );
                v.normal=vec3(1,0,0);
                v.position=vec3(x,y,z)     *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y,z+1)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y,z+1)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z)  *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z+1) *CHUNK_SIZE; vArray.push_back(v);
            }
        }
    }
    // Y
    for(int x=0;x<CHUNK_N;++x)
    for(int z=0;z<CHUNK_N;++z)
    for(int y=1;y<CHUNK_N;++y)
    {
        if (value[x][y][z])
        {
            if (!value[x][y-1][z])
            {
                GL_Vertex v;
                v.color=vec4(
                        double(x)/double(CHUNK_N),
                        double(y)/double(CHUNK_N),
                        double(z)/double(CHUNK_N),
                        1.0
                );
                v.normal=vec3(0,-1,0);
                v.position=vec3(x,y,z)     *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y,z+1)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z+1) *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y,z+1)  *CHUNK_SIZE; vArray.push_back(v);
            }
        }
        else
        {
            if (value[x][y-1][z])
            {
                GL_Vertex v;
                v.color=vec4(
                        double(x)/double(CHUNK_N),
                        double(y-1)/double(CHUNK_N),
                        double(z)/double(CHUNK_N),
                        1.0
                );
                v.normal=vec3(1,1,0);
                v.position=vec3(x,y,z)     *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y,z+1)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y,z+1)  *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z+1) *CHUNK_SIZE; vArray.push_back(v);
            }
        }
    }

    // Y
    for(int x=0;x<CHUNK_N;++x)
    for(int y=0;y<CHUNK_N;++y)
    for(int z=1;z<CHUNK_N;++z)
    {
        if (value[x][y][z])
        {
            if (!value[x][y][z-1])
            {
                GL_Vertex v;
                v.color=vec4(
                        double(x)/double(CHUNK_N),
                        double(y)/double(CHUNK_N),
                        double(z)/double(CHUNK_N),
                        1.0
                );
                v.normal=vec3(0,0,-1);
                v.position=vec3(x,y,z)     *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y+1,z) *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z)   *CHUNK_SIZE; vArray.push_back(v);
            }
        }
        else
        {
            if (value[x][y][z-1])
            {
                GL_Vertex v;
                v.color=vec4(
                        double(x)/double(CHUNK_N),
                        double(y)/double(CHUNK_N),
                        double(z-1)/double(CHUNK_N),
                        1.0
                );
                v.normal=vec3(1,1,0);
                v.position=vec3(x,y,z)     *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x,y+1,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y,z)   *CHUNK_SIZE; vArray.push_back(v);
                v.position=vec3(x+1,y+1,z) *CHUNK_SIZE; vArray.push_back(v);
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

TrackerPointer<Chunk>* Chunk::getTptr(void)
{
    return tptr;
}

void Chunk::initGLObjects()
{
    computeChunk();

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_Vertex)*vArray.size(), &(vArray[0]), GL_STATIC_DRAW);

    log_info("%d octet",sizeof(GL_Vertex) * vArray.size());
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
