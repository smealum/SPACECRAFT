#include "tests/testShaders.h"
#include "Application.h"
#include "render/Camera.h"

static GLfloat vertices[8*3*2] = {
        +0.5, -0.5, -0.5, 0.0 , 1.0 , 1.0 , 
        +0.5, +0.5, -0.5, 1.0 , 0.0 , 1.0 , 
        -0.5, +0.5, -0.5, 1.0 , 1.0 , 0.0 , 
        -0.5, -0.5, -0.5, 1.0 , 1.0 , 1.0 , 
        +0.5, -0.5, +0.5, 1.0 , 0.0 , 0.0 , 
        +0.5, +0.5, +0.5, 0.0 , 1.0 , 0.0 , 
        -0.5, +0.5, +0.5, 0.0 , 0.0 , 1.0 , 
        -0.5, -0.5, +0.5, 0.0 , 0.0 , 0.0 , 
};

static GLuint elements[12*3] = {
    0, 3, 2,
    0, 2, 1,
    0, 4, 7,
    0, 7, 3,
    1, 5, 4,
    1, 4, 0,
    2, 6, 5,
    2, 5, 1,
    3, 7, 6,
    3, 6, 2,
    4, 5, 6,
    4, 6, 7
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
