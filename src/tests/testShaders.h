#ifndef __TESTSHADERS_H__
#define __TESTSHADERS_H__

#include "utils/glm.h"
#include <render/Shader.h>

class testShaders {
    ShaderProgram &program;
    glm::mat4 model;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    public:
    testShaders();

    void update();
    void draw();


};
#endif
