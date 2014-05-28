#include "Input.h"
#include "Application.h"

#include <vector>
#include <cmath>
#include "dbg.h"
using namespace std;

map<int,pair<int,int> > keyState;
map<int,pair<int,int> > mouseState;
double m_mouseX;
double m_mouseY;
float horAngle = 0;
float verAngle = 0;
bool mouseIsFixed = false;
bool mouseIsFixing = false;

void Input::update(GLFWwindow* window)
{
    // get screen dimension
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // update key
    for(auto it = keyState.begin(); it!=keyState.end(); ++it )
    {
        it->second.second = it->second.first;
        it->second.first  = glfwGetKey(window,it->first);
    }

    // update mouse
    for(auto it = mouseState.begin(); it!=mouseState.end(); ++it )
    {
        it->second.second = it->second.first;
        it->second.first  = glfwGetMouseButton(window,it->first);
    }


    // petit fix
    if (mouseIsFixing)
    {
        glfwSetCursorPos(window, width/2, height/2);
        mouseIsFixing=false;
    }

    // get mouse position
    glfwGetCursorPos(window, &m_mouseX, &m_mouseY);

    // get mouse position (bis)
    horAngle = (float)(width/2 - m_mouseX);
    verAngle = (float)(height/2 - m_mouseY);

    // fix mousePosition
    if (mouseIsFixed)
    {
        glfwSetCursorPos(window, width/2, height/2);
        //glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
        //glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
    }
    else
    {
        //glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
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

float Input::getHorAngle()
{
    return horAngle;
}
float Input::getVerAngle() 
{
    return verAngle;
}

void Input::fixMouse()
{
    mouseIsFixed =true;
    mouseIsFixing=true;
}

void Input::unfixMouse()
{
    mouseIsFixed=false;
}
bool Input::isMouseFixed()
{
    return mouseIsFixed;
}
