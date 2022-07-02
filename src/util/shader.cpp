#include "shader.h"

namespace shader
{
    bool loadShader(const char *vertexShaderFileName, const char *fragmentShaderFileName, const char *geometryShaderFileName, GLuint *shaderProgram)
    {
        std::string vertexShaderSource = "";
        std::string fragmentShaderSource = "";
        std::string geometryShaderSource = "";

        // Load files first
        vertexShaderSource = util::readFile(vertexShaderFileName);
        fragmentShaderSource = util::readFile(fragmentShaderFileName);
        if (geometryShaderFileName != nullptr)
        {
            geometryShaderSource = util::readFile(geometryShaderFileName);
        }

        if (!vertexShaderSource.empty() && !fragmentShaderSource.empty())
        {
            const char *vertexShaderData = vertexShaderSource.c_str();
            const char *fragmentShaderData = fragmentShaderSource.c_str();
            const char *geometryShaderData = geometryShaderFileName != nullptr ? geometryShaderSource.c_str() : nullptr;
            return shader::compileShader(vertexShaderData, fragmentShaderData, geometryShaderData, shaderProgram);
        }
        else
        {
            std::cerr << "[ERROR] Shader source is empty (vertexShaderSource: " << vertexShaderSource.length()
                      << " Bytes, fragmentShaderSource: " << fragmentShaderSource.length() << " Bytes)" << std::endl;
            return false;
        }
        return true;
    }

    bool compileShader(const char *vertexShaderData, const char *fragmentShaderData, const char *geometryShaderData, GLuint *shaderProgram)
    {
        unsigned int vertexShaderObject, fragmentShaderObject, geometryShaderObject;
        // Compile all shader and check for errors
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderObject, 1, &vertexShaderData, NULL);
        glCompileShader(vertexShaderObject);

        if(!shader::checkCompileErrors(vertexShaderObject, "VERTEX"))
        {
            return false;
        }

        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderObject, 1, &fragmentShaderData, NULL);
        glCompileShader(fragmentShaderObject);

        if(!shader::checkCompileErrors(fragmentShaderObject, "FRAGMENT"))
        {
            return false;
        }

        if (geometryShaderData != nullptr)
        {
            geometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometryShaderObject, 1, &geometryShaderData, NULL);
            glCompileShader(geometryShaderObject);
            if(!shader::checkCompileErrors(geometryShaderObject, "GEOMETRY"))
            {
                return false;
            }
        }

        *shaderProgram = glCreateProgram();
        glAttachShader(*shaderProgram, vertexShaderObject);
        glAttachShader(*shaderProgram, fragmentShaderObject);
        if (geometryShaderData != nullptr)
        {
            glAttachShader(*shaderProgram, geometryShaderObject);
        }
        glLinkProgram(*shaderProgram);
        
        if(!shader::checkCompileErrors(*shaderProgram, "PROGRAM"))
        {
            return false;
        }

        // Clean up shader objects after compilation
        glDeleteShader(vertexShaderObject);
        glDeleteShader(fragmentShaderObject);
        if (geometryShaderData != nullptr)
        {
            glDeleteShader(geometryShaderObject);
        }

        return true;
    }

    bool checkCompileErrors(GLuint shaderObject, std::string shaderType)
    {
        int success;
        char infoLog[1024];
        if (shaderType != "PROGRAM")
        {
            glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shaderObject, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER: Compiler error: Type: " << shaderType << std::endl
                          << infoLog << std::endl;
                return false;
            }
        }
        else
        {
            glGetProgramiv(shaderObject, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shaderObject, 1024, NULL, infoLog);
                std::cout << "ERROR::Shader: Linker error: Type: " << shaderType << std::endl
                          << infoLog << std::endl;
                return false;
            }
        }
        return true;
    }

    void setFloat(GLuint shaderProgram, const char *name, float val)
    {
        glUniform1f(glGetUniformLocation(shaderProgram, name), val);
    }

    void setVec2(GLuint shaderProgram, const char *name, const glm::vec2 &val)
    {
        glUniform2f(glGetUniformLocation(shaderProgram, name), val.x, val.y);    
    }

    void setInt(GLuint shaderProgram, const char *name, float val)
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name), val);
    }
}
