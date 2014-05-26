#include "tests/testShaders.h"
#include "Application.h"
#include "render/Camera.h"

static GLfloat vertices[] = {
    // POSITION           | COLOR       | NORMAL
    // x positif
    +0.5 , -0.5 , -0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    +0.5 , +0.5 , -0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    +0.5 , +0.5 , +0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    +0.5 , -0.5 , +0.5 , 1.0 , 0.0 , 0.0 , 1.f, +1.0 , 0.0 , 0.0 ,
    // x negatif
    -0.5 , -0.5 , -0.5 , 0.0 , 1.0 , 0.0 , 1.f, -1.0 , 0.0 , 0.0 ,
    -0.5 , +0.5 , -0.5 , 0.0 , 1.0 , 0.0 , 1.f, -1.0 , 0.0 , 0.0 ,
    -0.5 , +0.5 , +0.5 , 0.0 , 1.0 , 0.0 , 1.f, -1.0 , 0.0 , 0.0 ,
    -0.5 , -0.5 , +0.5 , 0.0 , 1.0 , 0.0 , 1.f, -1.0 , 0.0 , 0.0 ,
    // y positif
    -0.5 , +0.5 , -0.5 , 0.0 , 0.0 , 1.0 , 1.f, 0.0 , +1.0 , 0.0 ,
    +0.5 , +0.5 , -0.5 , 0.0 , 0.0 , 1.0 , 1.f, 0.0 , +1.0 , 0.0 ,
    +0.5 , +0.5 , +0.5 , 0.0 , 0.0 , 1.0 , 1.f, 0.0 , +1.0 , 0.0 ,
    -0.5 , +0.5 , +0.5 , 0.0 , 0.0 , 1.0 , 1.f, 0.0 , +1.0 , 0.0 ,
    // y negatif
    -0.5 , -0.5 , -0.5 , 0.0 , 1.0 , 1.0 , 1.f, 0.0 , -1.0 , 0.0 ,
    +0.5 , -0.5 , -0.5 , 0.0 , 1.0 , 1.0 , 1.f, 0.0 , -1.0 , 0.0 ,
    +0.5 , -0.5 , +0.5 , 0.0 , 1.0 , 1.0 , 1.f, 0.0 , -1.0 , 0.0 ,
    -0.5 , -0.5 , +0.5 , 0.0 , 1.0 , 1.0 , 1.f, 0.0 , -1.0 , 0.0 ,
    // z positif
    -0.5 , -0.5 , +0.5 , 1.0 , 0.0 , 1.0 , 1.f, 0.0 , 0.0 , +1.0 ,
    +0.5 , -0.5 , +0.5 , 1.0 , 0.0 , 1.0 , 1.f, 0.0 , 0.0 , +1.0 ,
    +0.5 , +0.5 , +0.5 , 1.0 , 0.0 , 1.0 , 1.f, 0.0 , 0.0 , +1.0 ,
    -0.5 , +0.5 , +0.5 , 1.0 , 0.0 , 1.0 , 1.f, 0.0 , 0.0 , +1.0 ,
    // z negatif
    -0.5 , -0.5 , -0.5 , 1.0 , 1.0 , 0.0 , 1.f, 0.0 , 0.0 , -1.0 ,
    +0.5 , -0.5 , -0.5 , 1.0 , 1.0 , 0.0 , 1.f, 0.0 , 0.0 , -1.0 ,
    +0.5 , +0.5 , -0.5 , 1.0 , 1.0 , 0.0 , 1.f, 0.0 , 0.0 , -1.0 ,
    -0.5 , +0.5 , -0.5 , 1.0 , 1.0 , 0.0 , 1.f, 0.0 , 0.0 , -1.0 ,
};


static GLuint elements[12*3] = {
    0,1,2,      0,2,3, // face 1
    4,5,6,      4,6,7, // face 2
    8,9,10,     8,10,11, // face 3
    12,13,14,   12,14,15, // face 4
    16,17,18,   16,18,19, // face 5
    20,21,22,   20,22,23, // face 6
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

    GLint posAttrib = program.attribLocation("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 10*sizeof(GLfloat), 0);
    GLint colAttrib = program.attribLocation("color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE,
                          10*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    //GLint texAttrib = program.attribLocation("texcoord");
    //glEnableVertexAttribArray(texAttrib);
    //glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                           //8*sizeof(float), (void*)(6*sizeof(float)));
    
    program.setUniform("overrideColor", glm::vec4(1.f));
    program.setUniform("model", model);
}

void testShaders::draw()
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    program.use();
    Camera &cam(Application::getInstance().getCamera());
    cam.updateCamera(program);

    glDrawElements(
            GL_TRIANGLES,
            12,
            GL_UNSIGNED_INT,
            NULL
            );

}
