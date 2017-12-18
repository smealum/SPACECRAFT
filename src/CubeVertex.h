#ifndef _CUBE_VERTEX_H_
#define _CUBE_VERTEX_H_

#include "render/Shader.h"
#include "utils/glm.h"

struct CubeVertex {

  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texture_coordinate;
  float tile;

  static void SetAttributes(ShaderProgram* shader) {
    shader->setAttribute("position", 3, GL_FALSE, 9, 0);
    shader->setAttribute("normal", 3, GL_TRUE, 9, 3);
    shader->setAttribute("texture_coordinate", 2, GL_FALSE, 9, 6);
    shader->setAttribute("tile", 1, GL_FALSE, 9, 8);
  }
};

#endif /* end of include guard: __CUBE_VERTEX_H_ */
