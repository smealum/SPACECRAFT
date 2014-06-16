#include "Chunk.h"
#include "MiniWorld.h"
#include "data/ContentHandler.h"
#include "world/blockProcessing.h"
#include "utils/positionMath.h"
#include "utils/dbg.h"

using namespace std;
using namespace glm;

Chunk::Chunk(Planet* p, class MiniWorld* mw, int x, int y, int z, glm::vec3 v1, glm::vec3 v2, glm::vec3 origin):
    program(ShaderProgram::loadFromFile("shader/chunk/chunk.vert", "shader/chunk/chunk.frag", "chunk")),
    tptr(new TrackerPointer<Chunk>(this, true)),
    planet(p),
    miniWorld(mw),
    px(x),
    py(y),
    pz(z),
    v1(v1),
    v2(v2),
    modified(false),
    origin(origin),
	constructionCanceled(false)
{
    //fait dans miniworld maintenant
    // planet->handler.requestContent(new WorldChunkRequest(*planet, *this, mw->face->elevation, origin, v1, v2, x, y, z));

    memset(value,0,sizeof(chunkVal)*(CHUNK_N+2)*(CHUNK_N+2)*(CHUNK_N+2));

    int numBlocks=p->getNumBlocks();
    boundingVolume[0]=origin+(v1*float(px)+v2*float(pz))/float(numBlocks);
    boundingVolume[1]=origin+(v1*float(px+CHUNK_N)+v2*float(pz))/float(numBlocks);
    boundingVolume[2]=origin+(v1*float(px+CHUNK_N)+v2*float(pz+CHUNK_N))/float(numBlocks);
    boundingVolume[3]=origin+(v1*float(px)+v2*float(pz+CHUNK_N))/float(numBlocks);
    for(int i=0;i<4;i++)boundingVolume[i]=glm::normalize(boundingVolume[i]);
    for(int i=0;i<4;i++)boundingVolume[i+4]=boundingVolume[i]*(1.0f+float(py+CHUNK_N)/float(numBlocks));
    for(int i=0;i<4;i++)boundingVolume[i]*=1.0f+float(py)/float(numBlocks);

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
	constructionCanceled = true;
    tptr->release();
}

bool Chunk::isConstructionCanceled()
{
	return constructionCanceled;
}

//TEMP
extern int testTexture;
extern int testTextureArray;
extern int testVal;

void Chunk::draw(Camera& cam, glm::mat4 model)
{
    // if(!vArray.size())return;
    if(!vArray.size())return;

    if(!cam.isBoxInFrustum(boundingVolume, 8, model))return;

    testVal++; //TEMP
 
    program.use();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    cam.updateCamera(program);

    program.setUniform("origin",origin);
    program.setUniform("v1",v1);
    program.setUniform("v2",v2);
    program.setUniform("lightdir",planet->lightdir);
    program.setUniform("model",(model));

    //glBindTexture(GL_TEXTURE_2D, testTexture);
	// On n'a pas besoin de bind un textureArray? (uniquement utilisable dans un shader)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY,testTextureArray);
	program.setUniform("Texture",0);

    glDrawArrays(GL_TRIANGLES, 0, vArray.size());
}

TrackerPointer<Chunk>* Chunk::getTptr(void)
{
    return tptr;
}

//même principe que raymarching de http://www.cse.yorku.ca/~amana/research/grid.pdf
glm::i32vec3 Chunk::performRayMarch(glm::dvec3 localBlockPosf, glm::dvec3 localBlockPosf2, glm::dvec3* out, bool* done, int* dir)
{
    if(done)*done=false;
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
            if(done)*done=true;
            return cur;
        }
        if(tMaxX < tMaxY)
        {
            if(tMaxX < tMaxZ)
            {
                // printf("step X\n");
                cur.x+=stepX;
                tMaxX+=tDeltaX;
                if(dir)*dir=0;
                if(cur.x<0 || cur.x>CHUNK_N){if(out)*out=localBlockPosf+u*tMaxX; return cur;}
            }else{
                // printf("step Z\n");
                cur.z+=stepZ;
                tMaxZ+=tDeltaZ;
                if(dir)*dir=2;
                if(cur.z<0 || cur.z>CHUNK_N){if(out)*out=localBlockPosf+u*tMaxZ; return cur;}
            }   
        } else {
            if(tMaxY < tMaxZ) {
                // printf("step Y\n");
                cur.y+=stepY;
                tMaxY+=tDeltaY;
                if(dir)*dir=1;
                if(cur.y<0 || cur.y>CHUNK_N){if(out)*out=localBlockPosf+u*tMaxY; return cur;}
            }else{
                // printf("step Z\n");
                cur.z+=stepZ;
                tMaxZ+=tDeltaZ;
                if(dir)*dir=2;
                if(cur.z<0 || cur.z>CHUNK_N){if(out)*out=localBlockPosf+u*tMaxZ; return cur;}
            }
        }
    }while(value[cur.z+1][cur.y+1][cur.x+1]==blockTypes::air);

    if(done)*done=true;
    return cur;
}

bool Chunk::collidePoint(glm::dvec3& p, glm::dvec3& v)
{
    if(glm::length(v)<=1e-12)return false;

    bool ret=false;

    while(glm::length(v)>1e-12)
    {
        // TODO : optimiser en ne la calculant qu'une fois par toplevel (max) par frame ?
        glm::dvec3 blockPos=p;
        glm::dvec3 blockPos2=p+v;
        
        glm::dvec3 localBlockPosf=glm::dvec3(blockPos.x-px,blockPos.y-py,blockPos.z-pz);
        glm::dvec3 localBlockPosf2=glm::dvec3(blockPos2.x-px,blockPos2.y-py,blockPos2.z-pz);
        glm::i32vec3 localBlockPosi=glm::i32vec3(floor(blockPos.x)-px,floor(blockPos.y)-py,floor(blockPos.z)-pz);
        // glm::i32vec3 localBlockPosi2=glm::i32vec3(floor(blockPos2.x)-px,floor(blockPos2.y)-py,floor(blockPos2.z)-pz);
        
        if(localBlockPosf.x<0 || localBlockPosf.y<0 || localBlockPosf.z<0 ||
            localBlockPosf.x>=CHUNK_N || localBlockPosf.y>=CHUNK_N || localBlockPosf.z>=CHUNK_N)
            return ret;

        // printf("LENGTH1 %f (%d %d %d)\n",glm::length(v),localBlockPosi.z+1,localBlockPosi.y+1,localBlockPosi.x+1);

        if(value[localBlockPosi.z+1][localBlockPosi.y+1][localBlockPosi.x+1]!=blockTypes::air){return ret;}

        int dir;
        glm::i32vec3 cur=performRayMarch(localBlockPosf, localBlockPosf2, NULL, NULL, &dir);
        if(value[cur.z+1][cur.y+1][cur.x+1]==blockTypes::air)return ret;
        
        glm::dvec3 u=glm::normalize(localBlockPosf2-localBlockPosf);
        const double d=glm::length(localBlockPosf2-localBlockPosf);

        // printf("collision %d %d %d (%f %f %f) %f (%f %f %f)\n",cur.x,cur.y,cur.z,localBlockPosf.x,localBlockPosf.y,localBlockPosf.z,d);
        ret=true;
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
                    u.x=0;v=u*(d-r);
                }
                break;
            case 1:
                {
                    const int stepY=(localBlockPosf2.y>localBlockPosf.y)?1:-1;
                    double targetY=(cur.y+py)*1.0;
                    if(stepY<0)targetY+=1.0;
                    targetY-=0.01*stepY; //marge de 1cm
                    double r=(targetY-blockPos.y)/u.y;
                    blockPos+=u*r;
                    blockPos.y=targetY;
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
                    u.z=0;v=u*(d-r);
                }
                break;
        }
        p=blockPos;
    }
    return ret;
}

bool Chunk::selectBlock(glm::dvec3 p, glm::dvec3 v, glm::i32vec3& out, glm::dvec3& out2, bool& done, int& dir)
{
    glm::dvec3 blockPos=p;
    glm::dvec3 blockPos2=p+v;
    
    glm::dvec3 localBlockPosf=glm::dvec3(blockPos.x-px,blockPos.y-py,blockPos.z-pz);
    glm::dvec3 localBlockPosf2=glm::dvec3(blockPos2.x-px,blockPos2.y-py,blockPos2.z-pz);
    glm::i32vec3 localBlockPosi=glm::i32vec3(floor(blockPos.x)-px,floor(blockPos.y)-py,floor(blockPos.z)-pz);
    // glm::i32vec3 localBlockPosi2=glm::i32vec3(floor(blockPos2.x)-px,floor(blockPos2.y)-py,floor(blockPos2.z)-pz);
    
    if(localBlockPosf.x<-1 || localBlockPosf.y<-1 || localBlockPosf.z<-1 ||
        localBlockPosf.x>CHUNK_N || localBlockPosf.y>CHUNK_N || localBlockPosf.z>CHUNK_N)
        return false;

    // printf("BLOCK1 %f %f %f\n",blockPos.x,blockPos.y,blockPos.z);
    // printf("BLOCK2 %f %f %f\n",blockPos2.x,blockPos2.y,blockPos2.z);

    out=performRayMarch(localBlockPosf, localBlockPosf2, &out2, &done, &dir);
    if(value[out.z+1][out.y+1][out.x+1]==blockTypes::air)
    {
        if(!done)out2+=glm::dvec3(px,py,pz);
        return false;
    }

    dir*=2;
    switch(dir)
    {
        case 0: dir+=(localBlockPosf.x<localBlockPosf2.x)?(0):(1); break;
        case 2: dir+=(localBlockPosf.y<localBlockPosf2.y)?(0):(1); break;
        case 4: dir+=(localBlockPosf.z<localBlockPosf2.z)?(0):(1); break;
    }

    out+=glm::i32vec3(px,py,pz);

    return true;
}

void Chunk::changeBlock(glm::i32vec3 p, blockTypes::T v)
{
    p-=glm::i32vec3(px,py,pz);
    if(p.x<-1 || p.y<-1 || p.z<-1 || p.x>CHUNK_N || p.y>CHUNK_N || p.z>CHUNK_N)return;
    modified=true;
    if(value[p.z+1][p.y+1][p.x+1]==v)return;
    if(v!=blockTypes::air && value[p.z+1][p.y+1][p.x+1]!=blockTypes::air)return; //on peut soit créer soit supprimer, pas remplacer

    value[p.z+1][p.y+1][p.x+1]=v;

    //TODO (pas sûr qu'on puisse de façon efficace/utile) : mieux optimiser la modification du VBO ?
    //(de toute façon, changeBlock est un évènement *très* ponctuel, donc ça ne devrait pas géner)
    computeChunkFaces((chunkVal*)value, 1, 1, 1, 0, 0, 0, px, py, pz, origin, v1, v2, planet->getNumBlocks(), vArray);

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

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        program.setBuffers(vao, vbo, 0);
        program.use();
        glBindFragDataLocation(program.getHandle(), 0, "outColor");
        program.setAttribute("position", 3, GL_FALSE, 10, 0);
        program.setAttribute("elevation", 1, GL_FALSE, 10, 3);
        program.setAttribute("intexcoord", 2, GL_FALSE, 10, 4);
        program.setAttribute("tile", 1, GL_FALSE, 10, 6);
        program.setAttribute("normal", 3, GL_FALSE, 10, 7);
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
