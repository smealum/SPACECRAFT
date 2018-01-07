#include <iostream>
#include "Input.h"
#include "Application.h"

#include <vector>
#include <cmath>
#include "dbg.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

using namespace std;

map<int, pair<int, int>> keyState;
map<int, pair<int, int>> mouseState;
double m_mouseX;
double m_mouseY;
double m_mouseX_previous;
double m_mouseY_previous;
float horAngle = 0;
float verAngle = 0;
bool mouseIsFixed = false;
bool mouseIsFixing = false;

void Input::update(GLFWwindow* window) {
#ifdef __EMSCRIPTEN__
  mouseIsFixed = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;

  static bool initialized = false;
  if (!initialized) {
    initialized = true;

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
#endif

  // get screen dimension
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  // update key
  for (auto it = keyState.begin(); it != keyState.end(); ++it) {
    it->second.second = it->second.first;
    it->second.first = glfwGetKey(window, it->first);
  }

  // update mouse
  for (auto it = mouseState.begin(); it != mouseState.end(); ++it) {
    it->second.second = it->second.first;
    it->second.first = glfwGetMouseButton(window, it->first);
  }

//#ifndef __EMSCRIPTEN__
  //// petit fix
  //if (mouseIsFixing) {
    //glfwSetCursorPos(window, width / 2, height / 2);
    //mouseIsFixing = false;
  //}
//#endif

  // get mouse position
  glfwGetCursorPos(window, &m_mouseX, &m_mouseY);

  // get mouse position (bis)
  horAngle = (float)(m_mouseX_previous - m_mouseX);
  verAngle = (float)(m_mouseY_previous - m_mouseY);

#ifndef __EMSCRIPTEN__
  // fix mousePosition
  if (mouseIsFixed) {
    m_mouseX = width/2;
    m_mouseY = height/2;
    glfwSetCursorPos(window, m_mouseX, m_mouseY);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
#endif

  m_mouseX_previous = m_mouseX;
  m_mouseY_previous = m_mouseY;
}

bool Input::isKeyPressed(int key) {
  auto p = keyState[key];
  return ((p.first == GLFW_PRESS) and (p.second == GLFW_RELEASE));
}

bool Input::isKeyReleased(int key) {
  auto p = keyState[key];
  return ((p.first == GLFW_RELEASE) and (p.second == GLFW_PRESS));
}

bool Input::isKeyHold(int key) {
  auto p = keyState[key];
  return (p.first == GLFW_PRESS);
}

bool Input::isMousePressed(int key) {
  auto p = mouseState[key];
  return ((p.first == GLFW_PRESS) and (p.second == GLFW_RELEASE));
}

bool Input::isMouseReleased(int key) {
  auto p = mouseState[key];
  return ((p.first == GLFW_RELEASE) and (p.second == GLFW_PRESS));
}

bool Input::isMouseHold(int key) {
  auto p = mouseState[key];
  return (p.first == GLFW_PRESS);
}

double Input::mouseX() {
  return m_mouseX;
}

double Input::mouseY() {
  return m_mouseY;
}

float Input::getHorAngle() {
  return horAngle;
}

float Input::getVerAngle() {
  return verAngle;
}

void Input::fixMouse() {
#ifndef __EMSCRIPTEN__
  mouseIsFixed = true;
  mouseIsFixing = true;
#endif
}

void Input::unfixMouse() {
#ifndef __EMSCRIPTEN__
  mouseIsFixed = false;
#endif
}
bool Input::isMouseFixed() {
  return mouseIsFixed;
}
