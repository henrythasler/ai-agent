#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "util.h"

namespace shader
{
    void loadShader(const char *vertexShaderFile, const char *fragmentShaderFile, const char *geometryShaderFile, unsigned int *shaderProgram);
    void compileShader(const char *vertexShaderData, const char *fragmentShaderData, const char *geometryShaderData, unsigned int *shaderProgram);
    void checkCompileErrors(unsigned int shaderObject, std::string shaderType);
}
