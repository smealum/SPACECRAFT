#include "tests/testShaders.h"
#include "Application.h"
#include "render/Camera.h"

static GLfloat vertices[] = {
    -0.5f , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 0.0f ,
    0.5f  , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,
    0.5f  , 0.5f  , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 1.0f ,
    0.5f  , 0.5f  , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 1.0f ,
    -0.5f , 0.5f  , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f ,
    -0.5f , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 0.0f ,

    -0.5f , -0.5f , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 0.0f ,
    0.5f  , -0.5f , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,
    0.5f  , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 1.0f ,
    0.5f  , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 1.0f ,
    -0.5f , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f ,
    -0.5f , -0.5f , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 0.0f ,

    -0.5f , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,
    -0.5f , 0.5f  , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 1.0f ,
    -0.5f , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f ,
    -0.5f , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f ,
    -0.5f , -0.5f , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 0.0f ,
    -0.5f , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,

    0.5f  , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,
    0.5f  , 0.5f  , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 1.0f ,
    0.5f  , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f ,
    0.5f  , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f ,
    0.5f  , -0.5f , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 0.0f ,
    0.5f  , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,

    -0.5f , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f ,
    0.5f  , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 1.0f ,
    0.5f  , -0.5f , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,
    0.5f  , -0.5f , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,
    -0.5f , -0.5f , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 0.0f ,
    -0.5f , -0.5f , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f ,

    -0.5f , 0.5f  , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f ,
    0.5f  , 0.5f  , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 1.0f ,
    0.5f  , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,
    0.5f  , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 1.0f , 0.0f ,
    -0.5f , 0.5f  , 0.5f  , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 0.0f ,
    -0.5f , 0.5f  , -0.5f , 1.0f , 1.0f , 1.0f , 1.f, 0.0f , 1.0f 

};

testShaders::testShaders() :
    program(ShaderProgram::loadFromFile("shader/basic/basic.vert", "shader/basic/basic.frag")),
    model(1.f),
    vao(0),
    vbo(0)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    
}

void testShaders::update()
{
    Camera &cam(Application::getInstance().getCamera());
    cam.updateCamera(program);
}
