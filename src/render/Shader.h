#ifndef SHADER_F8X43H2W
#define SHADER_F8X43H2W

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utils/glm.h"
#include <string>
#include <map>
#include <list>

class Shader;
class ShaderProgram;

namespace ShaderType
{
    enum T {
        Vertex = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Geometry = GL_GEOMETRY_SHADER
    };
};

class Shader
{
    public:
        // Chargement du shader depuis un fichier
        static Shader& loadFromFile(const char *filename, ShaderType::T type);


        // Fournit l'identifiant du shader
        inline GLuint getHandle() { return handle; }
        

    private:
        // constructeur privé, passer par Shader::fromFile(filename)
        Shader();

        // identifiant du shader OpenGL
        GLuint handle;

        // constructeur par copie (private, interdit)
        Shader(const Shader& shader);
};

class ShaderProgram
{
    public:
        // constructeur
        static ShaderProgram& loadFromFile(const char* vertexShader, const char* fragmentShader);
        static ShaderProgram& loadFromShader(Shader& vertexShader, Shader& fragmentShader);

        // active le shader
        void use();

        // Fournit l'identifiant du program
        inline GLuint getHandle() { return handle; }

        // Fournit localisation d'un uniform.
        GLint uniform(const char* name);
        GLint attribLocation(const char* name);
        inline GLint operator[](const char* name) {return uniform(name);}

        // parametrer le ShaderProgram
        void setUniform(const char *name, float x,float y,float z);
        void setUniform(const char *name, const glm::vec3 & v);
        void setUniform(const char *name, const glm::vec4 & v);
        void setUniform(const char *name, const glm::mat4 & m);
        void setUniform(const char *name, const glm::mat3 & m);
        void setUniform(const char *name, float val );
        void setUniform(const char *name, int val );
        void setUniform(const char *name, bool val );

        // invalid program, need some shaders
        ShaderProgram();
        ~ShaderProgram();

        // attach a compiled shader
        void attachShader(Shader &s);
        void detachShader(Shader &s);
        // parametrer les attributs
        void setAttribute(const char *name, GLint size, GLboolean normalized, GLsizei stride, GLuint offset);
        //void bindAttribLocation( GLuint location,const char * name);
        //void bindFragDataLocation( GLuint location,const char * name );
    private:
        // identifiant OpenGL
        GLuint handle;
        bool valid; // does it need to be linked when there some shaders have been attached

        // identifiants des attributs.
        std::list<Shader*> attachedShaders;
        std::map<std::string,GLint> uniformsMap;

        // constructeur par copie (private, interdit)
        ShaderProgram(const ShaderProgram& other);
};

#endif /* end of include guard: SHADER_F8X43H2W */
