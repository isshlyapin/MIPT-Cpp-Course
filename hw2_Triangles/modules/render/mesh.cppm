module;

#include <span>
#include <memory>
#include <utility>
#include <stdexcept>

#include <GL/glew.h>

export module isshlyapin.render:mesh;

import :helpers;
import :exceptions;
import :shader_program;

namespace ogl {

export
class IMesh {
public:
    virtual ~IMesh() = default;
    virtual void draw() const = 0;

    IMesh(const IMesh&) = delete;
    IMesh& operator=(const IMesh&) = delete;
    IMesh(IMesh&&) = delete;
    IMesh& operator=(IMesh&&) = delete;

protected:
    IMesh() = default;
};

class GLBuffer {
public:
    GLBuffer() = default;
    explicit GLBuffer(GLenum target) : target_(target) {
        gl_call(glGenBuffers, 1, &id_);
    }

    ~GLBuffer() { glDeleteBuffers(1, &id_); }

    GLBuffer(const GLBuffer&) = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;

    GLBuffer(GLBuffer&& other) noexcept : GLBuffer() {
        buf_swap(other);
    }
    GLBuffer& operator=(GLBuffer&& other) noexcept {
        if (this != &other) { buf_swap(other); }
        return *this;
    }

    void bind() const { 
        gl_call(glBindBuffer, target_, id_);
    }

    void data(size_t bytes, const void* data, GLenum usage) {
        bind();
        gl_call(glBufferData, target_, bytes, data, usage);
        unbind();
    }

    template<typename T>
    void data(std::span<const T> data, GLenum usage) {
        data(data.size_bytes(), data.data(), usage);
    }

    void unbind() const { gl_call(glBindBuffer, target_, 0); }

private:
    void buf_swap(GLBuffer& other) noexcept {
        std::swap(id_, other.id_);
        std::swap(target_, other.target_);
    }

    GLuint id_{0};
    GLenum target_{0};
};

class GLVertexArray {
public:
    GLVertexArray() {
        gl_call(glGenVertexArrays, 1, &id_);
    }

    ~GLVertexArray() { glDeleteVertexArrays(1, &id_); }

    GLVertexArray(const GLVertexArray&) = delete;
    GLVertexArray& operator=(const GLVertexArray&) = delete;
    GLVertexArray(GLVertexArray&& other) noexcept : id_(other.id_) {
        other.id_ = 0;
    }
    GLVertexArray& operator=(GLVertexArray&& other) noexcept {
        if (this != &other) { std::swap(id_, other.id_); }
        return *this;
    }

    void bind() const { gl_call(glBindVertexArray, id_); }

    static void unbind()  { gl_call(glBindVertexArray, 0); }

    void attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
        bind();
        gl_call(glVertexAttribPointer, index, size, type, normalized, stride, pointer);
        gl_call(glEnableVertexAttribArray, index);
        unbind();
    }

private:
    GLuint id_{0};
};

export
class TrisMash : public IMesh {
public:
    TrisMash(std::span<const float> vertexes, 
             std::span<const float> normals, 
             std::span<const float> colors)
      : count_(vertexes.size() / 3), position_vbo_(GL_ARRAY_BUFFER), 
        normal_vbo_(GL_ARRAY_BUFFER), color_vbo_(GL_ARRAY_BUFFER) {
        if (vertexes.size() != normals.size() || vertexes.size() != colors.size() || vertexes.size() % 9 != 0) {
            throw std::invalid_argument(
                "Vertexes, normals and colors must have the same size \
                 and be a multiple of 9 (3 vertices per triangle, 3 components per vertex)"
            );
        }
        
        position_vbo_.data(vertexes, GL_STATIC_DRAW);
        position_vbo_.bind();
        vao_.attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), nullptr);
        
        normal_vbo_.data(normals, GL_STATIC_DRAW);
        normal_vbo_.bind();
        vao_.attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), nullptr);
        
        color_vbo_.data(colors, GL_STATIC_DRAW);
        color_vbo_.bind();
        vao_.attrib_pointer(2, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), nullptr);
    }
        
    void draw() const override {
        vao_.bind();
        gl_call(glDrawArrays, GL_TRIANGLES, 0, static_cast<GLsizei>(count_));
        GLVertexArray::unbind();
    };

private:
    size_t count_;
    std::shared_ptr<ShaderProgram> program_;
 
    GLBuffer position_vbo_;
    GLBuffer normal_vbo_;
    GLBuffer color_vbo_;
    GLVertexArray vao_;
};

export
struct Renderable {
    std::unique_ptr<IMesh> mesh;
    std::shared_ptr<ShaderProgram> program;
};

} // namespace ogl
