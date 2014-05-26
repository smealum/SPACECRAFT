#include "utils/glm.h"
#include <render/Shader.h>

class testShaders {
    ShaderProgram &program;
    glm::mat4 model;
    GLuint vao;
    GLuint vbo;

    testShaders();

    void update();
    void draw();

};
