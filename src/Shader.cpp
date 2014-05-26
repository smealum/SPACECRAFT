#include "Shader.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "utils/dbg.h"

using namespace std;

// collection des shader déja créés.
map<string,Shader> shaderMap;

// Lecture d'un fichier
bool getFileContents(const char *filename, vector<char>& buffer)
{
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

// chargement d'un shader depuis un fichier
Shader& Shader::loadFromFile(const char* filename, ShaderType::T type)
{
    Shader* s = new Shader();

    // chargement du fichier
    vector<char> fileContent;
    if (not getFileContents(filename,fileContent))
    {
        cerr << "[Erreur] Fichier " << filename << " introuvable"<<endl;
        exit(0);
    }

    // creation
    s->handle = glCreateShader(type);
    
    // assignation du code source
    glShaderSource(s->handle,1, (const GLchar**)&(fileContent[0]), NULL);

    // compilation
    glCompileShader(s->handle);

    // vérification de la compilation
    GLint compile_status;
    glGetShaderiv(s->handle, GL_COMPILE_STATUS, &compile_status);
    if(compile_status != GL_TRUE)
    {
        /* error text retreiving*/
        int logsize;
        glGetShaderiv(s->handle, GL_INFO_LOG_LENGTH, &logsize);
         
        char* log = new char[logsize+1];
        glGetShaderInfoLog(s->handle, logsize, &logsize, log);
        log[logsize]='\0';
         
        cerr<<"[erreur] Impossible de compiler le shader : "<<filename<<endl;
        cerr<<"============[Erreur]================="<<endl;
        cerr<<log<<endl;
        cerr<<"============[Erreur]================="<<endl;
        
        exit(0);
    }

    return *s;
}

Shader::Shader()
{

}
