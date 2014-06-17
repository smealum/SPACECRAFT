#ifndef SHADER_F8X43H2W
#define SHADER_F8X43H2W

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "utils/glm.h"
#include <string>
#include <map>
#include <list>
#include <set>

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
        inline GLuint getHandle() const { return handle; }
        inline std::string getFilename() const { return file; }

        void load();
        
    private:
        // constructeur privé, passez par Shader::fromFile(filename)
        Shader();

        // identifiant du shader OpenGL
        GLuint handle;
        ShaderType::T type;
        std::string file;
        std::set<ShaderProgram*> programs;

        // constructeur par copie (private, interdit)
        Shader(const Shader& shader);

        // when a program uses a shader, the shader must save a ref to  it 
        // in order to notify him if it is reloaded
        void addProgram(ShaderProgram* p);
        void removeProgram(ShaderProgram *p);

        void notifyPrograms();

        friend class ShaderProgram;

};


class ShaderProgram
{
    public:
        // constructeur
        static ShaderProgram& loadFromFile(const char* vertexShader, const char* fragmentShader, const std::string &name);
        static ShaderProgram& loadFromFile(const char* vertexShader, const char* fragmentShader, const char* geometryShader, const std::string &name);
        static ShaderProgram& loadFromShader(Shader& vertexShader, Shader& fragmentShader, const std::string &name);
        static ShaderProgram& loadFromShader(Shader& vertexShader, Shader& fragmentShader, Shader& geometryShader, const std::string &name);

        void setBuffers(GLint vao, GLint vbo, GLint ebo);

        // active le shader
        void use();

        // Fournit l'identifiant du program
        inline GLuint getHandle() const { return handle; }
        inline std::string getName() const { return name; }

        // Fournit localisation d'un uniform.
        GLint uniform(const char* name);
        GLint attribLocation(const char* name);
        inline GLint operator[](const char* name) {return uniform(name);}

        // parametrer le ShaderProgram
        void setUniform(const char *name, float x,float y,float z);
        void setUniform(const char *name, const glm::vec3 & v);
        void setUniform(const char *name, const glm::dvec3 & v);
        void setUniform(const char *name, const glm::vec4 & v);
        void setUniform(const char *name, const glm::dvec4 & v);
        void setUniform(const char *name, const glm::dmat4 & m);
        void setUniform(const char *name, const glm::mat4 & m);
        void setUniform(const char *name, const glm::mat3 & m);
        void setUniform(const char *name, float val );
        void setUniform(const char *name, int val );

        // invalid program, need some shaders
        ShaderProgram();
        ~ShaderProgram();

        inline void setInvalid() { valid = false; }

        // attach a compiled shader
        void attachShader(Shader &s);
        void detachShader(Shader &s);
        // parametrer les attributs
		// stride est multiplié par sizeof(float) en interne, de meme pour offset
        void setAttribute(const char *name, GLint size, GLboolean normalized, GLsizei stride, GLuint offset, GLenum type = GL_FLOAT);
        //void bindAttribLocation( GLuint location,const char * name);
        //void bindFragDataLocation( GLuint location,const char * name );
    private:
        struct uniform_t;
        struct attribute_t;
        std::map<std::string, uniform_t> uniforms;
        std::map<std::string, attribute_t> attributes;
        // identifiant OpenGL
        GLuint handle;
        bool valid; // does it need to be linked when there some shaders have been attached
        std::string name;
        GLint vao, vbo, ebo;

        // identifiants des attributs.
        std::set<Shader*> attachedShaders;
        std::map<std::string,GLint> uniformsMap;

        // constructeur par copie (private, interdit)
        ShaderProgram(const ShaderProgram& other);

        // used to save uniforms before reloading shader
        void saveUniforms();

        // load uniforms saved, used after linking new shaders
        void loadUniforms();

};

extern Shader* getShader(const std::string& file);
extern std::map<std::string,Shader*> shaderMap;


#endif /* end of include guard: SHADER_F8X43H2W */
