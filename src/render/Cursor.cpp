#include <iostream>
#include "render/Cursor.h"
#include "MiniWorld.h"
#include "Application.h"

static GLfloat vertices[] = {
    0.0f, 0.0f, 0.0f, // A
    0.0f, 0.0f, 1.0f, // B
    0.0f, 1.0f, 0.0f, // C
    0.0f, 1.0f, 1.0f, // D
};

Cursor::Cursor()
    : affected(false),
      planetModel(glm::mat4(1.0f)),
      shader(ShaderProgram::loadFromFile("shader/cursor/cursor.vert",
                                         "shader/cursor/cursor.frag",
                                         "cursor")),
      time(0.f) {
  // generate VBO
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  shader.setBuffers(vao, vbo, 0);

  shader.use();
  shader.setAttribute("offset_from_position", 3, GL_FALSE, 3, 0);
}

void Cursor::draw(Camera& c) {
  time += Application::getInstance().getFrameDeltaTime();

  if (!affected)
    return;

  shader.use();
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  c.updateCamera(shader);

  glm::mat4 model(1.0f);

  const float margin = 0.1f;

  std::cout << "dir = " << dir << std::endl;

  // TODO : passer dans un tableau de matrices constantes
  switch (dir) {
    case 0:
      model = glm::translate(glm::mat4(1.0f), glm::vec3(-margin, 0.0f, 0.0f)) *
              model;
      break;
    case 1:
      model = glm::rotate(model, PI, glm::vec3(0.0f, 1.0f, 0.0f));
      model =
          glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 1.0f)) * model;
      model = glm::translate(glm::mat4(1.0f), glm::vec3(margin, 0.0f, 0.0f)) *
              model;
      break;
    case 2:
      model = glm::rotate(model, PI / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
      model =
          glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * model;
      model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -margin, 0.0f)) *
              model;
      break;
    case 3:
      model = glm::rotate(model, -PI / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
      model =
          glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * model;
      model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, margin, 0.0f)) *
              model;
      break;
    case 4:
      model = glm::rotate(model, -PI / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
      model =
          glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * model;
      model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -margin)) *
              model;
      break;
    case 5:
      model = glm::rotate(model, PI / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
      model =
          glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * model;
      model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, margin)) *
              model;
      break;
  }

  shader.setUniform("model", model);
  shader.setUniform("planetModel", planetModel);
  shader.setUniform("origin", origin);
  shader.setUniform("v1", v1);
  shader.setUniform("v2", v2);
  shader.setUniform("position", glm::vec3(pos));
  shader.setUniform("numBlocks", float(numBlocks));
  shader.setUniform("t", time);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Cursor::setPosition(glm::i32vec3 pos,
                         int dir,
                         glm::vec3 origin,
                         glm::vec3 v1,
                         glm::vec3 v2,
                         glm::mat4 planetModel,
                         int numBlocks) {
  std::cout << __func__ << std::endl;
  affected = true;
  this->dir = dir;
  this->pos = pos;
  this->origin = origin;
  this->v1 = v1;
  this->v2 = v2;
  this->numBlocks = numBlocks;
  this->planetModel = planetModel;
}

void Cursor::unaffect(void) {
  affected = false;
}
