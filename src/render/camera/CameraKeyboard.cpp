#include "../Camera.h"
#include "galaxy/Galaxy.h"
#include "utils/Input.h"
#include "CameraKeyboard.h"
#include <GLFW/glfw3.h>
#include "utils/maths.h"
#include "Application.h"

using namespace std;
using namespace glm;

// clang-format off
void CameraKeyboard::update(Camera& camera)
{
  float delta = Application::getInstance().getFrameDeltaTime();
  
  // rotation ----------------------------------------------------------------
  auto rotate = [&](const glm::vec3& axe) {
    camera.view3 = mat3(glm::rotate(mat4(1.0), 1.5f * delta, axe)) * camera.view3;
  };
  if (Input::isKeyHold(GLFW_KEY_I)) rotate(glm::vec3(-1.0, -0.0, -0.0));
  if (Input::isKeyHold(GLFW_KEY_K)) rotate(glm::vec3(+1.0, +0.0, +0.0));
  if (Input::isKeyHold(GLFW_KEY_J)) rotate(glm::vec3(-0.0, -1.0, -0.0));
  if (Input::isKeyHold(GLFW_KEY_L)) rotate(glm::vec3(+0.0, +1.0, +0.0));
  if (Input::isKeyHold(GLFW_KEY_U)) rotate(glm::vec3(-0.0, -0.0, -1.0));
  if (Input::isKeyHold(GLFW_KEY_O)) rotate(glm::vec3(+0.0, +0.0, +1.0));

  // Changement de vitesse ---------------------------------------------------
  if (Input::isKeyPressed(GLFW_KEY_Y)) speed *= 4.0f;
  if (Input::isKeyPressed(GLFW_KEY_H)) speed /= 4.0f;
  
  // translation -------------------------------------------------------------
  glm::dvec3 translation(0.0, 0.0, 0.0);
  if (Input::isKeyHold(GLFW_KEY_D)) translation += dvec3(-1.0, -0.0, -0.0);
  if (Input::isKeyHold(GLFW_KEY_A)) translation += dvec3(+1.0, +0.0, +0.0);
  if (Input::isKeyHold(GLFW_KEY_E)) translation += dvec3(-0.0, -1.0, -0.0);
  if (Input::isKeyHold(GLFW_KEY_Q)) translation += dvec3(+0.0, +1.0, +0.0);
  if (Input::isKeyHold(GLFW_KEY_S)) translation += dvec3(-0.0, -0.0, -1.0);
  if (Input::isKeyHold(GLFW_KEY_W)) translation += dvec3(+0.0, +0.0, +1.0);
  
  camera.pos -= (translation * glm::dmat3(camera.view3)) * double(speed * delta);

  camera.updateView();
  camera.updateFrustum();
}
// clang-format on
