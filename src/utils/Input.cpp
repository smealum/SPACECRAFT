#include "Input.h"
#include "Application.h"

#include <vector>
#include <cmath>
#include "dbg.h"
using namespace std;

static map<int,pair<int,int> > keyState;
static map<int,pair<int,int> > mouseState;
static double m_mouseX;
static double m_mouseY;
bool Input::fixMouse(false);
float Input::horAngle(INFINITY), Input::verAngle(INFINITY);
glm::vec3 Input::position(0.f, 0.f, 0.f);

void Input::update(GLFWwindow* window)
{
    for(auto it = keyState.begin(); it!=keyState.end(); ++it )
    {
        it->second.second = it->second.first;
        it->second.first  = glfwGetKey(window,it->first);
    }
    for(auto it = mouseState.begin(); it!=mouseState.end(); ++it )
    {
        it->second.second = it->second.first;
        it->second.first  = glfwGetMouseButton(window,it->first);
    }
    glfwGetCursorPos(window, &m_mouseX, &m_mouseY);

    if (fixMouse)
    {
	float mSpeed = 0.05f;
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glfwSetCursorPos(window, width/2, height/2);
	if (horAngle == INFINITY)
	    horAngle = verAngle = 0.f;
	else 
	{
	    horAngle = mSpeed * 1.f * (float)(width/2 - m_mouseX);
	    verAngle = mSpeed * 1.f * (float)(height/2 - m_mouseY);
	}
	debug("mouse is %f, %f moves at speed %f, %f", m_mouseX, m_mouseY, horAngle, verAngle);
    }
}

bool Input::isKeyPressed(int key)
{
    auto p = keyState[key];
    return (( p.first  == GLFW_PRESS ) and
            ( p.second == GLFW_RELEASE ));
}
bool Input::isKeyReleased(int key)
{
    auto p = keyState[key];
    return (( p.first  == GLFW_RELEASE ) and
            ( p.second == GLFW_PRESS ));
}
bool Input::isKeyHold(int key)
{
    auto p = keyState[key];
    return ( p.first  == GLFW_PRESS );
}

bool Input::isMousePressed(int key)
{
    auto p = mouseState[key];
    return (( p.first  == GLFW_PRESS ) and
            ( p.second == GLFW_RELEASE ));
}
bool Input::isMouseReleased(int key)
{
    auto p = mouseState[key];
    return (( p.first  == GLFW_RELEASE ) and
            ( p.second == GLFW_PRESS ));
}
bool Input::isMouseHold(int key)
{
    auto p = mouseState[key];
    return ( p.first  == GLFW_PRESS);
}

double Input::mouseX()
{
    return m_mouseX;
}

double Input::mouseY()
{
    return m_mouseY;
}

void Input::setMousePos(double x, double y)
{
    glfwSetCursorPos(Application::getInstance().getWindow(), x, y);
}
