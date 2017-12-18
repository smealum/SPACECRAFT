#ifndef CHUNK_4TYUGC3N
#define CHUNK_4TYUGC3N

#define BLOCK_SIZE 1.0f
#define CHUNK_N 32
#define CHUNK_SIZE (BLOCK_SIZE * CHUNK_N)

#include "CubeVertex.h"
#include "Planet.h"
#include "render/Camera.h"
#include "utils/TrackerPointer.h"
#include "utils/glm.h"
#include "world/BlockType.h"

#include <vector>
typedef uint8_t chunkVal;

// do NOT instanciate directly use new.
// do NOT destroy directly use destroyChunk.
class Chunk {
  friend class CameraPlayerGround;

 public:
  Chunk(Planet* p,
        class MiniWorld* mw,
        int x,
        int y,
        int z,
        glm::vec3 v1,
        glm::vec3 v2,
        glm::vec3 n);

  void draw(Camera& camera, glm::mat4 model, bool reg = true);
  void updateData(chunkVal* data,
                  std::vector<CubeVertex>& vareg,
                  std::vector<CubeVertex>& valpha);
  void destroyChunk(void);

  glm::i32vec3 performRayMarch(glm::dvec3 localBlockPosf,
                               glm::dvec3 localBlockPosf2,
                               glm::dvec3* out,
                               bool* done,
                               int* dir);
  bool collidePoint(glm::dvec3& p, glm::dvec3& v);
  bool selectBlock(glm::dvec3 p,
                   glm::dvec3 v,
                   glm::i32vec3& out,
                   glm::dvec3& out2,
                   bool& done,
                   int& dir);

  void changeBlock(glm::i32vec3 p, blockTypes::T v);
  void deleteBlock(glm::i32vec3 p);

  TrackerPointer<Chunk>* getTptr(void);

  Chunk* neighbour[4];  // NULL <=> nothing
  chunkVal value[CHUNK_N + 2][CHUNK_N + 2][CHUNK_N + 2];

  bool isConstructionCanceled();

 private:
  void initGLObjects();
  void initGLObjects(std::vector<CubeVertex>& va, GLuint& vao, GLuint& vbo);
  void destroyGLObjects();
  void destroyGLObjects(GLuint& vao, GLuint& vbo);
  void draw(Camera& cam,
            glm::mat4 model,
            std::vector<CubeVertex>& va,
            GLuint vao,
            GLuint vbo);

  bool modified;
  bool constructionCanceled;

  int px, py, pz;
  glm::vec3 v1, v2, origin, n;
  glm::vec3 boundingVolume[8];

  TrackerPointer<Chunk>* tptr;
  Planet* planet;
  class MiniWorld* miniWorld;

  GLuint reg_vbo, reg_vao;
  GLuint alpha_vbo, alpha_vao;

  std::vector<CubeVertex> vArray;
  std::vector<CubeVertex> alpha_vArray;
  ShaderProgram& program;
};

#endif /* end of include guard: CHUNK_4TYUGC3N */
