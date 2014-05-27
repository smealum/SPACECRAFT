#include "render/Shader.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "utils/dbg.h"
#include <algorithm>

using namespace std;

// collection des shader déja créés.
map<string,Shader*> shaderMap;
// collection des programmes déjà créés.
map<string, ShaderProgram*> shaderProgramMap;

union ShaderProgram::uniform_u {
    glm::vec3 v3;
    glm::vec4 v4;
    glm::mat3 m3;
    glm::mat4 m4;
    float f;
    int i;
    uniform_u() {}
};
struct ShaderProgram::uniform_t {
    uniformType::T type;
    ShaderProgram::uniform_u val;
    uniform_t() {}
};

struct ShaderProgram::attribute_t {
    GLint size;
    GLboolean normalized;
    GLsizei stride;
    GLuint offset;
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
        log_err("============[Erreur log]========================\n%s\n================================================", log);
        
        //exit(EXIT_FAILURE);
    }
    else
    {
        log_info("Shader %s compiled successfully.", file.c_str());
    }

    notifyPrograms();
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

Shader::Shader()
{

}

Shader::Shader(const Shader& shader)
{
    handle = shader.handle;
}


ShaderProgram& ShaderProgram::loadFromFile(const char* vertexShader, const char* fragmentShader, const std::string &name)
{
    debug("Chargement d'un programme : %s , %s ",vertexShader,fragmentShader);
    return ShaderProgram::loadFromShader(
                Shader::loadFromFile(vertexShader,ShaderType::Vertex),
                Shader::loadFromFile(fragmentShader,ShaderType::Fragment),
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
         
        log_err("============[Erreur log]========================\n%s\n================================================", log);
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
            log_err("Uniform %s doesn't exist (value is %d).", name, r);
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
        log_err("Attibute %s doesn't exist.", name);

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
    uniforms[name].type = uniformType::v3;
    uniforms[name].val.v3 = glm::vec3(x, y, z);
}
void ShaderProgram::setUniform(const char *name, const glm::vec3 & v)
{
    glUniform3fv(uniform(name), 1, glm::value_ptr(v));
    uniforms[name].type = uniformType::v3;
    uniforms[name].val.v3 = v;
}
void ShaderProgram::setUniform(const char *name, const glm::vec4 & v)
{
    glUniform4fv(uniform(name), 1, glm::value_ptr(v));
    uniforms[name].type = uniformType::v4;
    uniforms[name].val.v4 = v;
}
void ShaderProgram::setUniform(const char *name, const glm::mat4 & m)
{
    glUniformMatrix4fv(uniform(name), 1, GL_FALSE, glm::value_ptr(m));
    uniforms[name].type = uniformType::m4;
    uniforms[name].val.m4 = m;
}
void ShaderProgram::setUniform(const char *name, const glm::mat3 & m)
{
    glUniformMatrix3fv(uniform(name), 1, GL_FALSE, glm::value_ptr(m));
    uniforms[name].type = uniformType::m3;
    uniforms[name].val.m3 = m;
}
void ShaderProgram::setUniform(const char *name, float val )
{
    glUniform1f(uniform(name), val);
    uniforms[name].type = uniformType::i;
    uniforms[name].val.f = val;
}
void ShaderProgram::setUniform(const char *name, int val )
{
    glUniform1i(uniform(name), val);
    uniforms[name].type = uniformType::i;
    uniforms[name].val.f = val;
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
        log_err("Cannot attach shader %s to program %s because it is alreayd attached...",
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

extern GLfloat elements[], vertices[];

void ShaderProgram::use()
{
    if (!valid)
    {
        valid = true;
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

            log_err("============[Erreur log]========================\n%s\n================================================", log);
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
                    GL_FLOAT,
                    it->second.normalized,
                    it->second.stride*sizeof(GLfloat),
                    (void*)(it->second.offset*sizeof(GLfloat))
                    );
            //log_info("Attrib %s at loc %d.", it->first.c_str(), loc);

        }
        log_info("Program %s is loaded again.", name.c_str());
        for (auto it(uniforms.begin()); it != uniforms.end(); ++it)
        {
            //debug("uniform of type %u.", it->second.type);
            switch (it->second.type) {
                case uniformType::v3:
                    glUniform3fv(uniform(it->first.c_str()), 1, glm::value_ptr(it->second.val.v3));
                    break;
                case uniformType::v4:
                    glUniform4fv(uniform(it->first.c_str()), 1, glm::value_ptr(it->second.val.v4));
                    break;
                case uniformType::m3:
                    glUniformMatrix3fv(uniform(it->first.c_str()), 1, GL_FALSE, glm::value_ptr(it->second.val.m3));
                    break;
                case uniformType::m4:
                    glUniformMatrix4fv(uniform(it->first.c_str()), 1, GL_FALSE, glm::value_ptr(it->second.val.m4));
                    break;
                case uniformType::f:
                    glUniform1f(uniform(it->first.c_str()), it->second.val.f);
                    break;
                case uniformType::i:
                    glUniform1i(uniform(it->first.c_str()), it->second.val.i);
                    break;
                default:
                    log_err("Unknow uniform type");
                    break;
            }
        }
    }
    glUseProgram(handle);
}

void ShaderProgram::setAttribute(const char *name, GLint size, GLboolean normalized, GLsizei stride, GLuint offset)
{
    GLint loc = attribLocation(name);
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(
            loc,
            size,
            GL_FLOAT,
            normalized,
            stride*sizeof(GLfloat),
            (void*)(offset*sizeof(GLfloat))
            );
    attributes[name].size = size;
    attributes[name].normalized = normalized;
    attributes[name].stride = stride;
    attributes[name].offset = offset;
}

void ShaderProgram::setBuffers(GLint vao, GLint vbo, GLint ebo)
{
    this->vao = vao;
    this->vbo = vbo;
    this->ebo = ebo;
}
