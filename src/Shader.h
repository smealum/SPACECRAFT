#ifndef SHADER_F8X43H2W
#define SHADER_F8X43H2W

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <map>

namespace ShaderType
{
    enum T {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESS_CONTROL,
        TESS_EVALUATION
    };
};

class Shader
{
    public:
        // Chargement du shader depuis un fichier
        static Shader& loadFromFile(const char *filename,ShaderType::T type);

        // Fournit localisation d'un uniform.
        GLuint uniform(const char* name);

        // Fournit l'identifiant du shader
        GLuint getHandle();
        

        //void bindAttribLocation( GLuint location,const char * name);
        //void bindFragDataLocation( GLuint location,const char * name );

        // parametrer le shader
        void setUniform(const char *name,float x,float y,float z);
        void setUniform(const char *name, const glm::vec3 & v);
        void setUniform(const char *name, const glm::vec4 & v);
        void setUniform(const char *name, const glm::mat4 & m);
        void setUniform(const char *name, const glm::mat3 & m);
        void setUniform(const char *name, float val );
        void setUniform(const char *name, int val );
        void setUniform(const char *name, bool val );

    private:
        Shader();   // constructeur privé, passer par Shader::fromFile(filename)
        GLuint handle;
        std::map<std::string,GLuint> uniformsMap;
};

#endif /* end of include guard: SHADER_F8X43H2W */
