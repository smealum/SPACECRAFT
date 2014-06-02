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

    boundingVolume[0]=origin+(v1*float(px)+v2*float(pz))/float(PLANETFACE_BLOCKS);
    boundingVolume[1]=origin+(v1*float(px+CHUNK_N)+v2*float(pz))/float(PLANETFACE_BLOCKS);
    boundingVolume[2]=origin+(v1*float(px+CHUNK_N)+v2*float(pz+CHUNK_N))/float(PLANETFACE_BLOCKS);
    boundingVolume[3]=origin+(v1*float(px)+v2*float(pz+CHUNK_N))/float(PLANETFACE_BLOCKS);
    for(int i=0;i<4;i++)boundingVolume[i]=glm::normalize(boundingVolume[i]);
    for(int i=0;i<4;i++)boundingVolume[i+4]=boundingVolume[i]*(1.0f+float(py+CHUNK_N)/float(PLANETFACE_BLOCKS));
    for(int i=0;i<4;i++)boundingVolume[i]*=1.0f+float(py)/float(PLANETFACE_BLOCKS);

    n=mw->face->toplevel->vertex[4];

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

//TEMP
extern int testTexture;
extern int testVal;

void Chunk::draw(Camera& cam, glm::mat4 model)
{
    if(!vArray.size())return;

    //TEMP CULLING

    if(!cam.isBoxInFrustum(boundingVolume, 8))return;

    testVal++; //TEMP
 
    program.use();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    cam.updateCamera(program);

    program.setUniform("origin",origin);
    program.setUniform("v1",v1);
    program.setUniform("v2",v2);
    program.setUniform("numBlocks",float(PLANETFACE_BLOCKS));

    glBindTexture(GL_TEXTURE_2D, testTexture);

    glDrawArrays(GL_POINTS, 0 ,  vArray.size()); 

    collidePoint(cam.getPosition());
}

TrackerPointer<Chunk>* Chunk::getTptr(void)
{
    return tptr;
}

bool Chunk::collidePoint(glm::vec3 p)
{
    glm::vec3 blockPos;
    glm::vec3 unprojectedPos=(p*(glm::dot(origin,n)/glm::dot(p,n)))-origin;
    //calcul de la position en blocs dans la toplevel
    //TODO : passer ce genre de calculs dans des fonctions helper ?
    //TODO : optimiser en ne la calculant qu'une fois par toplevel (max) par frame ?
    blockPos.x=(glm::dot(unprojectedPos,glm::normalize(v1))*PLANETFACE_BLOCKS)/glm::length(v1);
    blockPos.y=(glm::length(p)-1.0f)*PLANETFACE_BLOCKS;
    blockPos.z=(glm::dot(unprojectedPos,glm::normalize(v2))*PLANETFACE_BLOCKS)/glm::length(v2);
    
    glm::i32vec3 localBlockPos=glm::i32vec3(blockPos.x-px,blockPos.y-py,blockPos.z-pz);
    
    if(localBlockPos.x<0 || localBlockPos.y<0 || localBlockPos.z<0 ||
        localBlockPos.x>=CHUNK_N || localBlockPos.y>=CHUNK_N || localBlockPos.z>=CHUNK_N)
        return false;

    return false;
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
        program.setAttribute("position", 3, GL_FALSE, 6, 0);
        program.setAttribute("texcoord", 2, GL_FALSE, 6, 3);
        program.setAttribute("facedir", 1, GL_FALSE, 6, 5);
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
