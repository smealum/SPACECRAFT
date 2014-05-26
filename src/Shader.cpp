#include "Shader.h"
#include <vector>
#include <fstream>

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

Shader& Shader::loadFromFile(const char* filename, ShaderType::T type)
{
    Shader s;
    vector<char> fileContent;
    if (not getFileContents(filename,fileContent))
    {
    
    }
    return s;
}

Shader::Shader()
{

}
