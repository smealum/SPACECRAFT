#include "render/Camera.h"
#include "Application.h"
#include "utils/glm.h"
#include "utils/maths.h"

Camera::Camera() :
    view(1.f),
    proj(glm::perspective(DEG2RAD(45.f),
                Application::getInstance().getWindowRatio()
                , 0.1f, 1000.f))
{}

void Camera::updateCamera(ShaderProgram &prog)
{
    prog.setUniform("view", view);
    prog.setUniform("proj", proj);
}
