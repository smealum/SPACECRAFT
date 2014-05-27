#include "tests/testShaders.h"
#include "Application.h"
#include "render/camera/Camera.h"
#include "utils/dbg.h"
#include "utils/Input.h"

static GLfloat vertices[] = {
    //     POSITION    |      COLOR           |     NORMAL
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
    programBasic(ShaderProgram::loadFromFile("shader/basic/basic.vert", "shader/basic/basic.frag")),
    programPhong(ShaderProgram::loadFromFile("shader/phong/phong.vert", "shader/phong/phong.frag")),
    model(1.f),
    vaoBasic(0),
    vaoPhong(0),
    vbo(0)
{

    glGenBuffers(1, &vbo);
    log_info("Created VBO(%d)", vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    log_info("Created EBO(%d)", ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // basic programme configuration
    {
        glGenVertexArrays(1, &vaoBasic);
        log_info("Created vaoBasic(%d)", vaoBasic);
        glBindVertexArray(vaoBasic);

        programBasic.use();
        glBindFragDataLocation(programBasic.getHandle(), 0, "outColor");
        GLint posAttrib = programBasic.attribLocation("position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 10*sizeof(GLfloat), 0);
        GLint colAttrib = programBasic.attribLocation("color");
        glEnableVertexAttribArray(colAttrib);
        glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE,
                              10*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        GLint texAttrib = programBasic.attribLocation("texcoord");
        glEnableVertexAttribArray(texAttrib);
        glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
                               10*sizeof(GLfloat), (void*)(0*sizeof(GLfloat))); // TODO change texcoord
        
        programBasic.setUniform("overrideColor", glm::vec4(1.f));

        programBasic.setUniform("model", model);
    }
    // phong programme configuration
    {
        glGenVertexArrays(1, &vaoPhong);
        log_info("Created vaoPhong(%d)", vaoPhong);
        glBindVertexArray(vaoPhong);

        programPhong.use();
        glBindFragDataLocation(programPhong.getHandle(), 0, "outColor");
        GLint posAttrib = programPhong.attribLocation("position");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(
                posAttrib,
                3,
                GL_FLOAT,
                GL_FALSE,
                10*sizeof(GLfloat),
                (void*)0
        );
        GLint colAttrib = programPhong.attribLocation("color");
        glEnableVertexAttribArray(colAttrib);
        glVertexAttribPointer(
                colAttrib,
                4,
                GL_FLOAT,
                GL_FALSE,
                10*sizeof(GLfloat),
                (void*)(3*sizeof(GLfloat))
        );
        GLint normalAttrib = programPhong.attribLocation("normal");
        glEnableVertexAttribArray(normalAttrib);
        glVertexAttribPointer(
                normalAttrib,
                3,
                GL_FLOAT,
                GL_TRUE,
                10*sizeof(GLfloat),
                (void*)(7*sizeof(GLfloat))
        );
        
    }

    Camera &cam(Application::getInstance().getCamera());
    cam.view = glm::lookAt(
            glm::vec3(0, 0, -5.f),
            glm::vec3(0.f,0.f,0.f),
            glm::vec3(0, 1.f, 0.f)
            );
}

void testShaders::draw()
{
    static float modelAngle=0.0;
    modelAngle+=0.04;

    Camera &cam(Application::getInstance().getCamera());
    // basic program
    {
        programBasic.use();
        glBindVertexArray(vaoBasic);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        cam.updateCamera(programBasic);

        model = glm::translate(glm::mat4(1.0),-glm::vec3(-1.0,0.0,0.0));
        model = glm::rotate(model,modelAngle,glm::vec3(sin(modelAngle),0.0,1.0));
        programBasic.setUniform("model", model);

        glDrawElements(
                GL_TRIANGLES,
                36,
                GL_UNSIGNED_INT,
                0
                );
    }
    // phong program
    {
        model = glm::translate(glm::mat4(1.0),-glm::vec3(1.0,0.0,0.0));
        model = glm::rotate(model,modelAngle,glm::vec3(sin(modelAngle),0.0,1.0));

        programPhong.use();
        glBindVertexArray(vaoPhong);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        programPhong.setUniform("projection",cam.proj);
        programPhong.setUniform("model",model);
        programPhong.setUniform("view",cam.view);

        glDrawElements(
                GL_TRIANGLES,
                36,
                GL_UNSIGNED_INT,
                0
                );
    }
}
