#include "renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

Shader::Shader(const char* vertSrc, const char* fragSrc) {
    GLuint vert = compile(GL_VERTEX_SHADER,   vertSrc);
    GLuint frag = compile(GL_FRAGMENT_SHADER, fragSrc);

    m_id = glCreateProgram();
    glAttachShader(m_id, vert);
    glAttachShader(m_id, frag);
    glLinkProgram(m_id);

    GLint ok;
    glGetProgramiv(m_id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(m_id, 512, nullptr, log);
        glDeleteProgram(m_id);
        throw std::runtime_error(std::string("Shader link error: ") + log);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader() {
    glDeleteProgram(m_id);
}

void Shader::bind()   const { glUseProgram(m_id); }
void Shader::unbind() const { glUseProgram(0); }

void Shader::setMat4(const char* name, const glm::mat4& v) const {
    glUniformMatrix4fv(loc(name), 1, GL_FALSE, glm::value_ptr(v));
}

void Shader::setVec3(const char* name, const glm::vec3& v) const {
    glUniform3fv(loc(name), 1, glm::value_ptr(v));
}

void Shader::setFloat(const char* name, float v) const {
    glUniform1f(loc(name), v);
}

GLint Shader::loc(const char* name) const {
    return glGetUniformLocation(m_id, name);
}

GLuint Shader::compile(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        glDeleteShader(shader);
        throw std::runtime_error(std::string("Shader compile error: ") + log);
    }
    return shader;
}
