#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>

#include "util.h"

namespace shader
{
    bool loadShader(const char *vertexShaderFile, const char *fragmentShaderFile, const char *geometryShaderFile, GLuint *shaderProgram);
    bool compileShader(const char *vertexShaderData, const char *fragmentShaderData, const char *geometryShaderData, GLuint *shaderProgram);
    bool checkCompileErrors(unsigned int shaderObject, std::string shaderType);

    void setFloat(GLuint shaderProgram, const char *name, float value);
    void setVec2(GLuint shaderProgram, const char *name, const glm::vec2 &val);
    void setInt(GLuint shaderProgram, const char *name, float val);
}
