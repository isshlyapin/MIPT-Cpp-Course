module;

#include <string>
#include <utility>
#include <unordered_map>

#include <GL/glew.h>

export
module isshlyapin.render:shader_program;

import :helpers;
import :exceptions;

namespace ogl {

class Shader {
public:
    Shader() = default; 

    Shader(GLenum type, const GLchar* source) : id_(gl_call(glCreateShader, type)) {
        try {
            gl_call(glShaderSource, id_, 1, &source, nullptr);
            gl_call(glCompileShader, id_);

            if (!is_compile()) {
                const std::string log = get_log();
                throw ShaderCompileError("Failed to compile shader", log);
            }
        } catch (...) {
            glDeleteShader(id_);
            throw;
        }
    }

    ~Shader() { glDeleteShader(id_); }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept : id_(other.id_) { other.id_ = 0; }
    Shader& operator=(Shader&& other) noexcept {
        if (this != &other) { 
            std::swap(id_, other.id_);
        }
        return *this;
    }

    [[nodiscard]] GLuint id() const { return id_; }

private:
    [[nodiscard]] bool is_compile() const {
        GLint success = 0;
        glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
        return success == GL_TRUE;
    }

    [[nodiscard]] std::string get_log() const {
        GLint len = 0;
        glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &len);
        std::string log; log.resize(len);
        glGetShaderInfoLog(id_, len, nullptr, log.data());
        return log;
    }

    GLuint id_{0};
};

class Program {
public:
    Program() = default;

    Program(const Shader& vertex, const Shader& fragment) : id_(gl_call(glCreateProgram)) {
        try {
            gl_call(glAttachShader, id_, vertex.id());
            gl_call(glAttachShader, id_, fragment.id());
            gl_call(glLinkProgram, id_);

            if (!is_linked()) {
                const std::string log = get_log();
                throw ShaderLinkError("Failed to link shader program", log);
            }
        } catch (...) {
            glDeleteProgram(id_);
            throw;
        }
    }

    Program(const Shader& vertex, const Shader& fragment, const Shader& geometry) 
      : id_(gl_call(glCreateProgram)) {
        try {
            gl_call(glAttachShader, id_, vertex.id());
            gl_call(glAttachShader, id_, fragment.id());
            gl_call(glAttachShader, id_, geometry.id());
            gl_call(glLinkProgram, id_);

            if (!is_linked()) {
                const std::string log = get_log();
                throw ShaderLinkError("Failed to link shader program", log);
            }
        } catch (...) {
            glDeleteProgram(id_);
            throw;
        }
    }

    ~Program() { glDeleteProgram(id_); }

    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;

    Program(Program&& other) noexcept : id_(other.id_) { other.id_ = 0; }
    Program& operator=(Program&& other) noexcept {
        if (this != &other) { 
            std::swap(id_, other.id_);
        }
        return *this;
    }

    [[nodiscard]] GLuint id() const { return id_; }
    
private:
    [[nodiscard]] bool is_linked() const {
        GLint success = 0;
        glGetProgramiv(id_, GL_LINK_STATUS, &success);
        return success == GL_TRUE;
    }

    [[nodiscard]] std::string get_log() const {
        GLint len = 0;
        glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &len);
        std::string log; log.resize(len);
        glGetProgramInfoLog(id_, len, nullptr, log.data());
        return log;
    }

    GLuint id_{0};
};

export
class ShaderProgram {
public:
    ShaderProgram(const GLchar* vertex_src, const GLchar* fragment_src) {
        const Shader vertex{GL_VERTEX_SHADER, vertex_src};
        const Shader fragment{GL_FRAGMENT_SHADER, fragment_src};

        program_ = Program(vertex, fragment);
    }

    ShaderProgram(const GLchar* vertex_src, const GLchar* fragment_src, const GLchar* geometry_src) {
        const Shader vertex{GL_VERTEX_SHADER, vertex_src};
        const Shader fragment{GL_FRAGMENT_SHADER, fragment_src};
        const Shader geometry{GL_GEOMETRY_SHADER, geometry_src};

        program_ = Program(vertex, fragment, geometry);
    }

    [[nodiscard]] GLuint id() const { return program_.id(); }

    void use() const { glUseProgram(id()); }

    [[nodiscard]] GLint get_uniform_location(const std::string_view name) {
        const std::string key(name);
        if (const auto it = uniforms.find(key); it != uniforms.end()) {
            return it->second;
        }
        const GLint location = gl_call(glGetUniformLocation, id(), name.data());
        if (location == -1) {
            throw OGLUniformNotFound("Uniform '" + std::string(name) + "' not found in shader program");
        }
        uniforms[key] = location;
        return location;
    }
private:
    Program program_;
    std::unordered_map<std::string, GLint> uniforms;
};

} // namespace ogl