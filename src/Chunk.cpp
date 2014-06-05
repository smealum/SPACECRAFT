#include "Chunk.h"
#include "MiniWorld.h"
#include "data/ContentHandler.h"
#include "world/blockProcessing.h"
#include "utils/positionMath.h"
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
    //fait dans miniworld maintenant
    // planet->handler.requestContent(new WorldChunkRequest(*planet, *this, mw->face->elevation, origin, v1, v2, x, y, z));

    memset(value,0,sizeof(chunkVal)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2));

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

void Chunk::updateData(chunkVal* data, std::vector<GL_Vertex> va)
{
    memcpy(value,data,sizeof(chunkVal)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2));
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
    program.setUniform("lightdir",planet->lightdir);

    glBindTexture(GL_TEXTURE_2D, testTexture);

    glDrawArrays(GL_POINTS, 0 ,  vArray.size());
}

TrackerPointer<Chunk>* Chunk::getTptr(void)
{
    return tptr;
}

//même principe que raymarching de http://www.cse.yorku.ca/~amana/research/grid.pdf
glm::i32vec3 Chunk::performRayMarch(glm::dvec3 localBlockPosf, glm::dvec3 localBlockPosf2, int* dir)
{
    glm::i32vec3 cur(localBlockPosf);
    glm::dvec3 u=glm::normalize(localBlockPosf2-localBlockPosf);
    const double d=glm::length(localBlockPosf2-localBlockPosf);
    const int stepX=(localBlockPosf2.x>localBlockPosf.x)?1:-1;
    const int stepY=(localBlockPosf2.y>localBlockPosf.y)?1:-1;
    const int stepZ=(localBlockPosf2.z>localBlockPosf.z)?1:-1;
    const double tDeltaX=abs(1.0/u.x); // w/u.x
    const double tDeltaY=abs(1.0/u.y); // h/u.y
    const double tDeltaZ=abs(1.0/u.z); // z/u.z

    double tMaxX, tMaxY, tMaxZ;

    if(abs(u.x)<0.001)tMaxX=d;
    else tMaxX=abs((localBlockPosf.x-floorf(localBlockPosf.x)+((localBlockPosf2.x>localBlockPosf.x)?-1.0:0.0))/u.x);

    if(abs(u.y)<0.001)tMaxY=d;
    else tMaxY=abs((localBlockPosf.y-floorf(localBlockPosf.y)+((localBlockPosf2.y>localBlockPosf.y)?-1.0:0.0))/u.y);

    if(abs(u.z)<0.001)tMaxZ=d;
    else tMaxZ=abs((localBlockPosf.z-floorf(localBlockPosf.z)+((localBlockPosf2.z>localBlockPosf.z)?-1.0:0.0))/u.z);

    // printf("precollision %d %d %d (%f %f %f) %f (%f %f %f)\n",cur.x,cur.y,cur.z,fabs(u.x),fabs(u.y),fabs(u.z),d,tMaxX,tMaxY,tMaxZ);

    do{
        if(tMaxX>=d && tMaxY>=d && tMaxZ>=d)
        {
            // on a parcouru tout v, donc on s'arrête
            // printf("no collision %d %d %d (%f %f %f) %f (%f %f %f)\n\n",cur.x,cur.y,cur.z,localBlockPosf.x,localBlockPosf.y,localBlockPosf.z,d,tMaxX,tMaxY,tMaxZ);
            // printf("no collision %d %d %d (%f %f %f) %f (%f %f %f)\n\n",cur.x,cur.y,cur.z,localBlockPosf2.x,localBlockPosf2.y,localBlockPosf2.z,d,tMaxX,tMaxY,tMaxZ);
            return cur;
        }
        if(tMaxX < tMaxY)
        {
            if(tMaxX < tMaxZ)
            {
                // printf("step X\n");
                cur.x+=stepX;
                if(dir)*dir=0;
                if(cur.x<0 || cur.x>CHUNK_N)return cur;
                tMaxX+=tDeltaX;
            }else{
                // printf("step Z\n");
                cur.z+=stepZ;
                if(dir)*dir=2;
                if(cur.z<0 || cur.z>CHUNK_N)return cur;
                tMaxZ+=tDeltaZ;
            }   
        } else {
            if(tMaxY < tMaxZ) {
                // printf("step Y\n");
                cur.y+=stepY;
                if(dir)*dir=1;
                if(cur.y<0 || cur.y>CHUNK_N)return cur;
                tMaxY+=tDeltaY;
            }else{
                // printf("step Z\n");
                cur.z+=stepZ;
                if(dir)*dir=2;
                if(cur.z<0 || cur.z>CHUNK_N)return cur;
                tMaxZ+=tDeltaZ;
            }
        }
    }while(value[cur.z+1][cur.y+1][cur.x+1]==blockTypes::air);
    return cur;
}

bool Chunk::collidePoint(glm::dvec3& p, glm::dvec3& v)
{
    if(glm::length(v)<=1e-12)return false;

    bool ret=false;

    while(glm::length(v)>1e-12)
    {
        // TODO : optimiser en ne la calculant qu'une fois par toplevel (max) par frame ?
        glm::dvec3 blockPos=dspaceToBlock(glm::dvec3(p),glm::dvec3(origin),glm::dvec3(v1),glm::dvec3(v2),glm::dvec3(n));
        glm::dvec3 blockPos2=dspaceToBlock(glm::dvec3(p+v),glm::dvec3(origin),glm::dvec3(v1),glm::dvec3(v2),glm::dvec3(n));
        
        glm::dvec3 localBlockPosf=glm::dvec3(blockPos.x-px,blockPos.y-py,blockPos.z-pz);
        glm::dvec3 localBlockPosf2=glm::dvec3(blockPos2.x-px,blockPos2.y-py,blockPos2.z-pz);
        glm::i32vec3 localBlockPosi=glm::i32vec3(floor(blockPos.x)-px,floor(blockPos.y)-py,floor(blockPos.z)-pz);
        // glm::i32vec3 localBlockPosi2=glm::i32vec3(floor(blockPos2.x)-px,floor(blockPos2.y)-py,floor(blockPos2.z)-pz);
        
        if(localBlockPosf.x<0 || localBlockPosf.y<0 || localBlockPosf.z<0 ||
            localBlockPosf.x>=CHUNK_N || localBlockPosf.y>=CHUNK_N || localBlockPosf.z>=CHUNK_N)
            return ret;

        // printf("LENGTH1 %f\n",glm::length(v));

        if(value[localBlockPosi.z+1][localBlockPosi.y+1][localBlockPosi.x+1]!=blockTypes::air){return ret;}

        int dir;
        glm::i32vec3 cur=performRayMarch(localBlockPosf, localBlockPosf2, &dir);
        if(value[cur.z+1][cur.y+1][cur.x+1]==blockTypes::air)return ret;
        
        glm::dvec3 u=glm::normalize(localBlockPosf2-localBlockPosf);
        const double d=glm::length(localBlockPosf2-localBlockPosf);

        // printf("collision %d %d %d (%f %f %f) %f (%f %f %f)\n",cur.x,cur.y,cur.z,localBlockPosf.x,localBlockPosf.y,localBlockPosf.z,d,tMaxX,tMaxY,tMaxZ);
        switch(dir)
        {
            case 0:
                {
                    const int stepX=(localBlockPosf2.x>localBlockPosf.x)?1:-1;
                    double targetX=(cur.x+px)*1.0;
                    if(stepX<0)targetX+=1.0;
                    targetX-=0.01*stepX; //marge de 1cm
                    double r=(targetX-blockPos.x)/u.x;
                    blockPos+=u*r;
                    blockPos.x=targetX;
                    p=dblockToSpace(blockPos, dvec3(origin), dvec3(v1), dvec3(v2));
                    u.x=0;v=u*(d-r);
                }
                break;
            case 1:
                {
                    const int stepY=(localBlockPosf2.y>localBlockPosf.y)?1:-1;
                    double targetY=(cur.y+py)*1.0;
                    ret=true;
                    if(stepY<0)targetY+=1.0;
                    targetY-=0.01*stepY; //marge de 1cm
                    double r=(targetY-blockPos.y)/u.y;
                    blockPos+=u*r;
                    blockPos.y=targetY;
                    p=dblockToSpace(blockPos, dvec3(origin), dvec3(v1), dvec3(v2));
                    u.y=0;v=u*(d-r);
                }
                break;
            default:
                {
                    const int stepZ=(localBlockPosf2.z>localBlockPosf.z)?1:-1;
                    double targetZ=(cur.z+pz)*1.0;
                    if(stepZ<0)targetZ+=1.0;
                    targetZ-=0.01*stepZ; //marge de 1cm
                    double r=(targetZ-blockPos.z)/u.z;
                    blockPos+=u*r;
                    blockPos.z=targetZ;
                    p=dblockToSpace(blockPos, dvec3(origin), dvec3(v1), dvec3(v2));
                    u.z=0;v=u*(d-r);
                }
                break;
        }
        v=dblockToSpace(blockPos+v, dvec3(origin), dvec3(v1), dvec3(v2))-p;
    }
    return ret;
}

bool Chunk::selectBlock(glm::dvec3 p, glm::dvec3 v, glm::i32vec3& out)
{
    // TODO : optimiser en ne la calculant qu'une fois par toplevel (max) par frame ?
    glm::dvec3 blockPos=dspaceToBlock(glm::dvec3(p),glm::dvec3(origin),glm::dvec3(v1),glm::dvec3(v2),glm::dvec3(n));
    glm::dvec3 blockPos2=dspaceToBlock(glm::dvec3(p+v),glm::dvec3(origin),glm::dvec3(v1),glm::dvec3(v2),glm::dvec3(n));
    
    glm::dvec3 localBlockPosf=glm::dvec3(blockPos.x-px,blockPos.y-py,blockPos.z-pz);
    glm::dvec3 localBlockPosf2=glm::dvec3(blockPos2.x-px,blockPos2.y-py,blockPos2.z-pz);
    glm::i32vec3 localBlockPosi=glm::i32vec3(floor(blockPos.x)-px,floor(blockPos.y)-py,floor(blockPos.z)-pz);
    // glm::i32vec3 localBlockPosi2=glm::i32vec3(floor(blockPos2.x)-px,floor(blockPos2.y)-py,floor(blockPos2.z)-pz);
    
    if(localBlockPosf.x<0 || localBlockPosf.y<0 || localBlockPosf.z<0 ||
        localBlockPosf.x>=CHUNK_N || localBlockPosf.y>=CHUNK_N || localBlockPosf.z>=CHUNK_N)
        return false;

    // printf("BLOCK1 %f %f %f\n",blockPos.x,blockPos.y,blockPos.z);
    // printf("BLOCK2 %f %f %f\n",blockPos2.x,blockPos2.y,blockPos2.z);

    int dir;
    out=performRayMarch(localBlockPosf, localBlockPosf2, &dir);
    if(value[out.z+1][out.y+1][out.x+1]==blockTypes::air)return false;

    out+=glm::i32vec3(px,py,pz);

    return true;
}

void Chunk::changeBlock(glm::i32vec3 p, blockTypes::T v)
{
    p-=glm::i32vec3(px,py,pz);
    if(p.x<-1 || p.y<-1 || p.z<-1 || p.x>CHUNK_N || p.y>CHUNK_N || p.z>CHUNK_N)return;
    if(value[p.z+1][p.y+1][p.x+1]==v)return;
    if(v!=blockTypes::air && value[p.z+1][p.y+1][p.x+1]!=blockTypes::air)return; //on peut soit créer soit supprimer, pas remplacer

    value[p.z+1][p.y+1][p.x+1]=v;

    //TODO (pas sûr qu'on puisse de façon efficace/utile) : mieux optimiser la modification du VBO ?
    //(de toute façon, changeBlock est un évènement *très* ponctuel, donc ça ne devrait pas géner)
    computeChunkFaces((chunkVal*)value, 1, 1, 1, 0, 0, 0, px, py, pz, vArray);

    destroyGLObjects();
    initGLObjects();
}

void Chunk::deleteBlock(glm::i32vec3 p)
{
    changeBlock(p,blockTypes::air);
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
