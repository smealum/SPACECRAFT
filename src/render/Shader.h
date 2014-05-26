#ifndef SHADER_F8X43H2W
#define SHADER_F8X43H2W

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utils/glm.h"
#include <string>
#include <map>

class Shader;
class ShaderProgram;

namespace ShaderType
{
    enum T {
        Vertex,
        Fragment,
        Geometry
    };
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
        GLuint getHandle();

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

        // parametrer les attributs
        //void bindAttribLocation( GLuint location,const char * name);
        //void bindFragDataLocation( GLuint location,const char * name );
    private:
        // constructeur (privé) . Utiliser loadFrom*
        ShaderProgram();

        // identifiant OpenGL
        GLuint handle;

        // identifiants des attributs.
        std::map<std::string,GLint> uniformsMap;

        // constructeur par copie (private, interdit)
        ShaderProgram(const ShaderProgram& other);
};

class Shader
{
    public:
        // Chargement du shader depuis un fichier
        static Shader& loadFromFile(const char *filename, ShaderType::T type);


        // Fournit l'identifiant du shader
        GLuint getHandle();
        

    private:
        // constructeur privé, passer par Shader::fromFile(filename)
        Shader();

        // identifiant du shader OpenGL
        GLuint handle;

        // constructeur par copie (private, interdit)
        Shader(const Shader& shader);
};

#endif /* end of include guard: SHADER_F8X43H2W */
