#include "shader.h"

namespace shader
{
    void loadShader(const char *vertexShaderFileName, const char *fragmentShaderFileName, const char *geometryShaderFileName, unsigned int *shaderProgram)
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
            shader::compileShader(vertexShaderData, fragmentShaderData, geometryShaderData, shaderProgram);
        }
        else
        {
            std::cerr << "shader source is empty (vertexShaderSource: " << vertexShaderSource.length()
                      << " Bytes, fragmentShaderSource: " << fragmentShaderSource.length() << " Bytes)" << std::endl;
        }
    }

    void compileShader(const char *vertexShaderData, const char *fragmentShaderData, const char *geometryShaderData, unsigned int *shaderProgram)
    {
        unsigned int vertexShaderObject, fragmentShaderObject, geometryShaderObject;
        // Compile all shader and check for errors
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShaderObject, 1, &vertexShaderData, NULL);
        glCompileShader(vertexShaderObject);
        shader::checkCompileErrors(vertexShaderObject, "VERTEX");

        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShaderObject, 1, &fragmentShaderData, NULL);
        glCompileShader(fragmentShaderObject);
        shader::checkCompileErrors(fragmentShaderObject, "FRAGMENT");

        if (geometryShaderData != nullptr)
        {
            geometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometryShaderObject, 1, &geometryShaderData, NULL);
            glCompileShader(geometryShaderObject);
            shader::checkCompileErrors(geometryShaderObject, "GEOMETRY");
        }

        *shaderProgram = glCreateProgram();
        glAttachShader(*shaderProgram, vertexShaderObject);
        glAttachShader(*shaderProgram, fragmentShaderObject);
        if (geometryShaderData != nullptr)
        {
            glAttachShader(*shaderProgram, geometryShaderObject);
        }
        glLinkProgram(*shaderProgram);
        shader::checkCompileErrors(*shaderProgram, "PROGRAM");

        // Clean up shader objects after compilation
        glDeleteShader(vertexShaderObject);
        glDeleteShader(fragmentShaderObject);
        if (geometryShaderData != nullptr)
        {
            glDeleteShader(geometryShaderObject);
        }
    }

    void checkCompileErrors(unsigned int shaderObject, std::string shaderType)
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
            }
        }
    }
}
