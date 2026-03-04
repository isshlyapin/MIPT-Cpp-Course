module;

#include <string>
#include <functional>

#include <GL/glew.h>


export module isshlyapin.render:helpers;

import :exceptions;

namespace ogl {

void ogl_check_error() {
    const auto error = glGetError();
    switch (error) {
        case GL_NO_ERROR:
            break;
        case GL_INVALID_ENUM:
            throw OGLInvalidEnum("GL_INVALID_ENUM: \
                An unacceptable value is specified for an enumerated argument"
            );
        case GL_INVALID_VALUE:
            throw OGLInvalidValue("GL_INVALID_VALUE: \
                A numeric argument is out of range"
            );
        case GL_INVALID_OPERATION:
            throw OGLInvalidOperation("GL_INVALID_OPERATION: \
                The specified operation is not allowed in the current state"
            );
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            throw OGLInvalidFramebufferOperation("GL_INVALID_FRAMEBUFFER_OPERATION: \
                The framebuffer object is not complete"
            );
        case GL_OUT_OF_MEMORY:
            throw OGLOutOfMemory("GL_OUT_OF_MEMORY: \
                There is not enough memory left to execute the command"
            );
        case GL_STACK_UNDERFLOW:
            throw OGLStackUnderflow("GL_STACK_UNDERFLOW: \
                An attempt has been made to perform an operation that would cause an internal stack to underflow"
            );
        case GL_STACK_OVERFLOW:
            throw OGLStackOverflow("GL_STACK_OVERFLOW: \
                An attempt has been made to perform an operation that would cause an internal stack to overflow"
            );
        default:
            using namespace std::string_literals;
            throw OGLError("Unknown OpenGL error: "s + std::to_string(error));
    }
}

export
template<typename F, typename... Args>
decltype(auto) gl_call(F&& f, Args&&... args) {
    using R = std::invoke_result_t<F, Args...>;

    if constexpr (std::is_void_v<R>) {
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    } else {
        auto result = std::invoke(
            std::forward<F>(f),
            std::forward<Args>(args)...
        );
        ogl_check_error();
        return result;
    }
    ogl_check_error();
}

export 
void glfw_error_callback(int error, const char* description) {
    using namespace std::string_literals;
    throw GLFWError("GLFW error: "s + std::to_string(error), description);
} 

} // namespace ogl
