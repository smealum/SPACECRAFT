#ifndef __TESTSHADERS_H__
#define __TESTSHADERS_H__

#include "utils/glm.h"
#include <render/Shader.h>
#include "Chunk.h"

class testShaders {
    ShaderProgram &programBasic, &programPhong;
    glm::mat4 model;
    GLuint vaoBasic;
    GLuint vaoPhong;
    GLuint vbo;
    GLuint ebo;
    Chunk chunk;


    public:
    testShaders();

    void update();
    void draw();


};
#endif
