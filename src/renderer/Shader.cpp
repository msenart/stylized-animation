#include "renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <regex>

static const std::string SHADER_FOLDER_PATH = "./shaders/";

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

Shader::Shader(const char* vertSrc, const char* fragSrc,
               const char* geomSrc, const char* tescSrc, const char* teseSrc)
    : Shader(buildRasterProgram(vertSrc, fragSrc, geomSrc, tescSrc, teseSrc)) {}

Shader Shader::compute(const char* compSrc) {
    return Shader(buildComputeProgram(compSrc));
}

Shader Shader::fromFiles(const std::string& vertPath, const std::string& fragPath,
                         const std::string& geomPath, const std::string& tescPath,
                         const std::string& tesePath) {
    std::string vert = readFile(vertPath);
    std::string frag = readFile(fragPath);
    std::string geom = geomPath.empty() ? std::string{} : readFile(geomPath);
    std::string tesc = tescPath.empty() ? std::string{} : readFile(tescPath);
    std::string tese = tesePath.empty() ? std::string{} : readFile(tesePath);
    return Shader(vert.c_str(), frag.c_str(),
                  geom.empty() ? nullptr : geom.c_str(),
                  tesc.empty() ? nullptr : tesc.c_str(),
                  tese.empty() ? nullptr : tese.c_str());
}

Shader Shader::computeFile(const std::string& compPath) {
    std::string src = readFile(compPath);
    return Shader::compute(src.c_str());
}

Shader::Shader(Shader&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        glDeleteProgram(m_id);
        m_id = std::exchange(other.m_id, 0);
    }
    return *this;
}

Shader::~Shader() { glDeleteProgram(m_id); }

// ---------------------------------------------------------------------------
// Bind / unbind
// ---------------------------------------------------------------------------

void Shader::bind()   const { glUseProgram(m_id); }
void Shader::unbind() const { glUseProgram(0); }

// ---------------------------------------------------------------------------
// Uniform setters
// ---------------------------------------------------------------------------

void Shader::set(const char* name, const glm::mat4& v) const {
    glUniformMatrix4fv(loc(name), 1, GL_FALSE, glm::value_ptr(v));
}
void Shader::set(const char* name, const glm::mat3& v) const {
    glUniformMatrix3fv(loc(name), 1, GL_FALSE, glm::value_ptr(v));
}
void Shader::set(const char* name, const glm::vec4& v) const {
    glUniform4fv(loc(name), 1, glm::value_ptr(v));
}
void Shader::set(const char* name, const glm::vec3& v) const {
    glUniform3fv(loc(name), 1, glm::value_ptr(v));
}
void Shader::set(const char* name, const glm::vec2& v) const {
    glUniform2fv(loc(name), 1, glm::value_ptr(v));
}
void Shader::set(const char* name, float v) const { glUniform1f(loc(name), v); }
void Shader::set(const char* name, int   v) const { glUniform1i(loc(name), v); }
void Shader::set(const char* name, bool  v) const { glUniform1i(loc(name), static_cast<int>(v)); }

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

GLint Shader::loc(const char* name) const {
    return glGetUniformLocation(m_id, name);
}

std::string Shader::readFile(const std::string& path) {
    std::string content;
    std::set<std::string> visited{};
    int counter = 0;
    content+=readRecursive(path,visited,counter);
    std::cout << content << std::endl;
    return content;
}

std::string Shader::readRecursive(const std::string& path, std::set<std::string>& visited, int& counter){
    std::string total_path = SHADER_FOLDER_PATH + path;
    auto cur_id = counter;
    std::string content;
    if (visited.count(total_path) > 0) {
        return "\n";
    }
    visited.insert(total_path);
    // read file
    std::ifstream f(total_path);
    if (!f.is_open())
        throw std::runtime_error("Shader: cannot open '" + total_path + "'");
    std::stringstream input_ss;
    std::stringstream output_ss;
    input_ss << f.rdbuf();

    std::regex regex = std::regex(R"(#include\s*[<"](.+)[>"])");
    std::smatch m;
    std::string line;
    int line_counter = 1;
    while (std::getline(input_ss, line)) {
        // deal with version directive
        if (line.find("#version") != std::string::npos && counter != 0) {
            continue;
        }
        // deal with include directive
        else if (std::regex_search(line, m, regex)) {
            auto next_path = m[1].str();
            ++counter;
            output_ss << "#line " << 1 << " " << counter << "\n";
            output_ss << readRecursive(next_path, visited,counter) << '\n';
            output_ss << "#line " << (line_counter + 1) << " " << cur_id << "\n";
        }
        else {
            output_ss << line << '\n';
        }
        line_counter++;
    }
    content+=output_ss.str();
    return content;
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

GLuint Shader::buildRasterProgram(const char* vertSrc, const char* fragSrc,
                                   const char* geomSrc, const char* tescSrc,
                                   const char* teseSrc) {
    // Compile all requested stages; on exception, already-compiled shaders
    // are cleaned up via the catch block below.
    GLuint stages[5] = {0, 0, 0, 0, 0};
    try {
        stages[0] = compile(GL_VERTEX_SHADER,          vertSrc);
        stages[1] = compile(GL_FRAGMENT_SHADER,        fragSrc);
        if (geomSrc) stages[2] = compile(GL_GEOMETRY_SHADER,        geomSrc);
        if (tescSrc) stages[3] = compile(GL_TESS_CONTROL_SHADER,    tescSrc);
        if (teseSrc) stages[4] = compile(GL_TESS_EVALUATION_SHADER, teseSrc);
    } catch (...) {
        for (GLuint s : stages) if (s) glDeleteShader(s);
        throw;
    }

    GLuint prog = glCreateProgram();
    for (GLuint s : stages) if (s) glAttachShader(prog, s);
    glLinkProgram(prog);

    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        glDeleteProgram(prog);
        for (GLuint s : stages) if (s) glDeleteShader(s);
        throw std::runtime_error(std::string("Shader link error: ") + log);
    }

    for (GLuint s : stages) if (s) glDeleteShader(s);
    return prog;
}

GLuint Shader::buildComputeProgram(const char* compSrc) {
    GLuint comp = compile(GL_COMPUTE_SHADER, compSrc);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, comp);
    glLinkProgram(prog);

    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        glDeleteProgram(prog);
        glDeleteShader(comp);
        throw std::runtime_error(std::string("Shader link error: ") + log);
    }

    glDeleteShader(comp);
    return prog;
}