#include "render/Shader.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "utils/dbg.h"
#include "utils/gldbg.h"
#include <algorithm>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

// collection des shader déja créés.
map<string,Shader*> shaderMap;
// collection des programmes déjà créés.
map<string, ShaderProgram*> shaderProgramMap;

struct uniform_u { // XXX should be an union
    GLfloat *v;
    GLint i; // for int types;
    GLint size;
    uniform_u () : v(NULL) {}
    //uniform_u(const uniform_u &u) : m4(u.m4) {}
};

struct ShaderProgram::uniform_t {
    GLenum type;
    uniform_u val;
    uniform_t() {}
    //uniform_t(const uniform_t& u) :  type(u.type), val(u.val){}
};


struct ShaderProgram::attribute_t {
    GLint size;
    GLboolean normalized;
    GLsizei stride;
    GLuint offset;
    GLenum type;
};

Shader* getShader(const std::string& file)
{
    auto it(shaderMap.find(file));
    return it != shaderMap.end() ? it->second : NULL;
}

// Lecture d'un fichier
bool getFileContents(const char *filename, vector<char>& buffer)
{
    //debug("chargement du fichier : %s",filename);
    ifstream file(filename, ios_base::binary);
    if (file)
    {
        file.seekg(0, ios_base::end);
        streamsize size = file.tellg();
        if (size > 0)
        {
            file.seekg(0, ios_base::beg);
            buffer.resize(static_cast<size_t>(size));
            file.read(&buffer[0], size);
        }
        buffer.push_back('\0');
        return true;
    }
    else
    {
        return false;
    }
}

void Shader::addProgram(ShaderProgram *p)
{
    programs.insert(p);
}

void Shader::removeProgram(ShaderProgram *p)
{
    programs.erase(p);
}

void Shader::notifyPrograms()
{
    for (auto it(programs.begin()); it != programs.end(); ++it)
    {
        (*it)->setInvalid();
    }
}

void Shader::load()
{
	glCheckError("Flush previous Errors");	

    // chargement du fichier
    vector<char> fileContent;
    if (!getFileContents(file.c_str(),fileContent))
    {
        log_err("[Erreur] Fichier %s  introuvable.", file.c_str());
        //exit(EXIT_FAILURE);
    }

    // creation
    if (handle == 0)
        handle = glCreateShader(type);
    if(handle == 0)
    {
        debug("Impossible de créer un shader vierge");
    }

    // assignation du code source
    const char* shaderText(&fileContent[0]);
    glShaderSource(handle, 1, (const GLchar**)&shaderText, NULL);

    // compilation
    glCompileShader(handle);

    // vérification de la compilation
    GLint compile_status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compile_status);
    if(compile_status != GL_TRUE)
    {
        /* error text retreiving*/
        GLsizei logsize = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logsize);
         
        char* log = new char[logsize+1];
        glGetShaderInfoLog(handle, logsize, &logsize, log);
        //log[logsize]='\0';
         
        log_err("Impossible de compiler le shader : %s",file.c_str());
        log_err("\n[Erreur log]\n%s\n_________", log);
        
        exit(EXIT_FAILURE);
    }
    else
    {
        log_info("Shader %s compiled successfully.", file.c_str());
    }

    notifyPrograms();

	glCheckError("Shader load");
}

// chargement d'un shader depuis un fichier
Shader& Shader::loadFromFile(const char* filename, ShaderType::T type)
{
    //debug("Chargement du shader : %s",filename);

    // test si le shader est déja chargé en mémoire.
    auto it = shaderMap.find(string(filename));
    if (it!=shaderMap.end())
        return *(it->second);

    // Sinon, on le charge
    Shader* s = new Shader;
    s->file = filename;
    s->type = type;

    s->load();

    // ajout du shader dans la map
    shaderMap[string(filename)]=s;

    return *s;
}

Shader::Shader() :
handle(0)
{
}

Shader::Shader(const Shader& shader) :
handle(shader.handle)
{
}


ShaderProgram& ShaderProgram::loadFromFile(const char* vertexShader, const char* fragmentShader, const std::string &name)
{
    // debug("Chargement d'un programme : %s , %s ",vertexShader,fragmentShader);
    return ShaderProgram::loadFromShader(
                Shader::loadFromFile(vertexShader,ShaderType::Vertex),
                Shader::loadFromFile(fragmentShader,ShaderType::Fragment),
                name
            );
}

ShaderProgram& ShaderProgram::loadFromFile(const char* vertexShader, const char* fragmentShader, const char* geometryShader, const std::string &name)
{
    // debug("Chargement d'un programme : %s , %s, %s ",vertexShader,fragmentShader,geometryShader);
    return ShaderProgram::loadFromShader(
                Shader::loadFromFile(vertexShader,ShaderType::Vertex),
                Shader::loadFromFile(fragmentShader,ShaderType::Fragment),
                Shader::loadFromFile(geometryShader,ShaderType::Geometry),
                name
            );
}

ShaderProgram& ShaderProgram::loadFromShader(Shader& vertexShader, Shader& fragmentShader, const std::string &name)
{
    // Check if shaderprogram exists
    auto it(shaderProgramMap.find(name));
    if (it != shaderProgramMap.end())
        return *(it->second);

    // programme creation
    ShaderProgram* p = new ShaderProgram;
    p->handle = glCreateProgram();
    if (not p->handle)
    {
        log_err("Impossible de créer un ShaderProgramme vierge");
        //exit(EXIT_FAILURE);
    }
    p->name = name;
    shaderProgramMap[p->name] = p;

    // attachement.
    p->attachShader(vertexShader);
    p->attachShader(fragmentShader);

    // linkage
    glLinkProgram(p->handle);
    p->valid = true;
    GLint result;
    glGetProgramiv(p->handle,GL_LINK_STATUS, &result);
    if (result!=GL_TRUE)
    {
        log_err("[Erreur] Impossible de linker les shader");

        /* error text retreiving*/
        GLsizei logsize = 0;
        glGetProgramiv(p->handle, GL_INFO_LOG_LENGTH, &logsize);
         
        char* log = new char[logsize];
        glGetProgramInfoLog(p->handle, logsize, &logsize, log);
        //log[logsize]='\0';
         
        log_err("\n[Erreur log]\n%s\n_________", log);
    }


    return *p;
}

ShaderProgram& ShaderProgram::loadFromShader(Shader& vertexShader, Shader& fragmentShader, Shader& geometryShader, const std::string &name)
{
    // Check if shaderprogram exists
    auto it(shaderProgramMap.find(name));
    if (it != shaderProgramMap.end())
        return *(it->second);

    // programme creation
    ShaderProgram* p = new ShaderProgram;
	p->handle = glCreateProgram();
    if (not p->handle)
    {
        log_err("Impossible de créer un ShaderProgramme vierge");
        //exit(EXIT_FAILURE);
    }
    p->name = name;
    shaderProgramMap[p->name] = p;

    // attachement.
    p->attachShader(vertexShader);
    p->attachShader(geometryShader);
    p->attachShader(fragmentShader);

    // linkage
    glLinkProgram(p->handle);
    p->valid = true;
    GLint result;
    glGetProgramiv(p->handle,GL_LINK_STATUS, &result);
    if (result!=GL_TRUE)
    {
        log_err("[Erreur] Impossible de linker les shader");

        /* error text retreiving*/
        GLsizei logsize = 0;
        glGetProgramiv(p->handle, GL_INFO_LOG_LENGTH, &logsize);
         
        char* log = new char[logsize];
        glGetProgramInfoLog(p->handle, logsize, &logsize, log);
        //log[logsize]='\0';
         
        log_err("\n[Erreur log]\n%s\n_________", log);
    }


    return *p;
}

GLint ShaderProgram::uniform(const char* name)
{
    auto it = uniformsMap.find(name);
    if (it == uniformsMap.end())
    {
        // uniforme non référencé
        GLint r = glGetUniformLocation(handle, name); 
        if ( r == GL_INVALID_OPERATION || r < 0 )
            log_warn("Uniform %s doesn't exist (value is %d) in program %s.", name, r, this->name.c_str());
        // add it anyways
        uniformsMap[name] = r;

        return r;
    }
    else
        return it->second; 
}

GLint ShaderProgram::attribLocation(const char *name)
{
    GLint attrib = glGetAttribLocation(handle, name);
    if (attrib == GL_INVALID_OPERATION || attrib < 0 )
        log_warn("Attribute %s doesn't exist in program %s.", name, this->name.c_str());

    return attrib;
}

ShaderProgram::ShaderProgram(const ShaderProgram& other)
{
    handle = other.handle;
    uniformsMap = other.uniformsMap;
}

void ShaderProgram::setUniform(const char *name,float x,float y,float z)
{
    glUniform3f(uniform(name), x, y, z);
}
void ShaderProgram::setUniform(const char *name, const vec3 & v)
{
    glUniform3fv(uniform(name), 1, value_ptr(v));
}
void ShaderProgram::setUniform(const char *name, const dvec3 & v)
{
    glUniform3dv(uniform(name), 1, value_ptr(v));
}
void ShaderProgram::setUniform(const char *name, const vec4 & v)
{
    glUniform4fv(uniform(name), 1, value_ptr(v));
}
void ShaderProgram::setUniform(const char *name, const dvec4 & v)
{
    glUniform4dv(uniform(name), 1, value_ptr(v));
}
void ShaderProgram::setUniform(const char *name, const dmat4 & m)
{
    glUniformMatrix4dv(uniform(name), 1, GL_FALSE, value_ptr(m));
}
void ShaderProgram::setUniform(const char *name, const mat4 & m)
{
    glUniformMatrix4fv(uniform(name), 1, GL_FALSE, value_ptr(m));
}
void ShaderProgram::setUniform(const char *name, const mat3 & m)
{
    glUniformMatrix3fv(uniform(name), 1, GL_FALSE, value_ptr(m));
}
void ShaderProgram::setUniform(const char *name, float val )
{
    glUniform1f(uniform(name), val);
}
void ShaderProgram::setUniform(const char *name, int val )
{
    glUniform1i(uniform(name), val);
}

ShaderProgram::ShaderProgram() :
    handle(0),
    valid(false)
{
}

ShaderProgram::~ShaderProgram()
{
    // detach shaders and set delete flag
    for (auto it(attachedShaders.begin()); it != attachedShaders.end(); ++it)
    {
        glDetachShader(handle, (*it)->getHandle());
        glDeleteShader((*it)->getHandle());
    }
    glDeleteProgram(handle);
}

void ShaderProgram::attachShader(Shader &s)
{
    auto it(attachedShaders.find(&s));
    if (it == attachedShaders.end())
    {
        s.addProgram(this);
        attachedShaders.insert(&s);
        glAttachShader(handle, s.getHandle());
        valid = false;
    } else
        log_err("Cannot attach shader %s to program %s because it is already attached...",
                s.getFilename().c_str(),
                name.c_str());
}

void ShaderProgram::detachShader(Shader &s)
{
    if (attachedShaders.erase(&s) > 0)
    {
        s.removeProgram(this);
        glDetachShader(handle, s.getHandle());
        glDeleteShader(s.getHandle()); // flags for deletion
        valid = false;
    } else
        log_err("Cannot detach shader %s from program %s...",
                s.getFilename().c_str(),
                name.c_str());
}

void ShaderProgram::use()
{
    if (!valid)
    {
        valid = true;
        glUseProgram(handle); // TODO is it really necessary?
        saveUniforms();
        // recheck uniforms
        glLinkProgram(handle);
        glUseProgram(handle);
        GLint result;
        glGetProgramiv(handle, GL_LINK_STATUS, &result);
        if (result!=GL_TRUE)
        {
            log_err("[Erreur] Impossible de linker les shader");

            /* error text retreiving*/
            GLsizei logsize = 0;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logsize);

            char* log = new char[logsize];
            glGetProgramInfoLog(handle, logsize, &logsize, log);
            //log[logsize]='\0';

            log_err("\n[Erreur log]\n%s\n_________", log);
        }
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        for (auto it(attributes.begin()); it != attributes.end(); ++it)
        {
            GLint loc = attribLocation(it->first.c_str());
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(
                    loc,
                    it->second.size,
                    it->second.type,
                    it->second.normalized,
                    it->second.stride*sizeof(GLfloat),
                    (void*)(it->second.offset*sizeof(GLfloat))
                    );
            //log_info("Attrib %s at loc %d.", it->first.c_str(), loc);

        }
        log_info("Program %s is loaded again.", name.c_str());
        loadUniforms();
	} else {
		glUseProgram(handle);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	}
}

void ShaderProgram::setAttribute(const char *name, GLint size, GLboolean normalized, GLsizei stride, GLuint offset, GLenum type)
{
    GLint loc = attribLocation(name);
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(
            loc,
            size,
            type,
            normalized,
            stride*sizeof(GLfloat),
            (void*)(offset*sizeof(GLfloat))
	);
    attributes[name].size = size;
    attributes[name].normalized = normalized;
    attributes[name].stride = stride;
    attributes[name].offset = offset;
    attributes[name].type = type;
}

void ShaderProgram::setBuffers(GLint vao, GLint vbo, GLint ebo)
{
    this->vao = vao;
    this->vbo = vbo;
    this->ebo = ebo;
}

void ShaderProgram::saveUniforms()
{
    GLint numUniforms = 0;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &numUniforms);
    log_info("There is %d uniforms in shader %s", numUniforms, name.c_str());
    //glGetProgramInterfaceiv(handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
    //const GLenum properties[4] = {GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION};

    for(int unif = 0; unif < numUniforms; ++unif)
    {
        //GLint values[4];
        GLchar name[124];
        GLsizei length;
        GLint size, loc;
        GLenum type;
        glGetActiveUniform(handle,
                    unif,
                    124,
                    &length,
                    &size,
                    &type,
                    name
                );
        loc = glGetUniformLocation(handle, name);
        log_info("uniform %d(loc: %d) of size %d, type %d and name %s", unif, loc, size, type, name);
        uniforms[name].type = type;
        auto &val = uniforms[name].val;
        debug("val: %p", val.v);
        if (val.v)
        {
            delete[] val.v;
            val.v = NULL;
        }
        val.size = size;
        debug("val: %p", val.v);
        for (int i = 0; i <size; i++)
        {
            switch (type) {
                case GL_FLOAT_MAT4:
                    if (!val.v)
                        val.v = new GLfloat[size*16];
                    glGetUniformfv(handle, loc, val.v+i*16);
                    break;
                case GL_FLOAT_VEC4:
                    if (!val.v)
                        val.v = new GLfloat[size*4];
                    glGetUniformfv(handle, loc+i, val.v+i*4);
                    debug("v[%d]=(%f,%f,%f,%f)", i,
                            val.v[0+i*4], val.v[1+i*4], val.v[2+i*4], val.v[3+i*4]);
                    break;
                case GL_FLOAT_VEC3:
                    if (!val.v)
                        val.v = new GLfloat[size*3];
                    glGetUniformfv(handle, loc+i, val.v+i*3);
                    break;
                case GL_FLOAT:
                    if (!val.v)
                        val.v = new GLfloat[size];
                    glGetUniformfv(handle, loc+i, val.v+i);
                    break;

                default:
                    log_err("Uniform %s of type %d is not supported!", name, type);
                    break;
            }
        }
    }
}

void ShaderProgram::loadUniforms()
{
    for (auto it(uniforms.begin()); it != uniforms.end(); ++it)
    {
        //debug("uniform of type %u.", it->second.type);
        auto &val(it->second.val);
        switch (it->second.type) {
            case GL_FLOAT_VEC4:
                //glUniform4fv(uniform(it->first.c_str()), 1, value_ptr(it->second.val.v4));
                glUniform4fv(uniform(it->first.c_str()),
                        val.size,
                        val.v);
                break;
            case GL_FLOAT:
                glUniform1fv(uniform(it->first.c_str()),
                        val.size,
                        val.v);
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(uniform(it->first.c_str()),
                        val.size,
                        GL_FALSE,
                        val.v);
                break;
            default:
                log_err("Uniform %s have a unsupported type!", it->first.c_str());
                break;
        }
    }

}

