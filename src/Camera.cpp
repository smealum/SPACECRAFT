#include "Camera.h"
#include "Application.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

Camera::Camera() :
    view(1.f),
    proj(glm::perspective(45.0f,
                Application::getInstance().getWindowRatio()
                , 0.1f, 1000.f))
{}

