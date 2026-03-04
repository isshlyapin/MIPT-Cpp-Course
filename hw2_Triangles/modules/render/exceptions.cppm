module;

#include <stdexcept>

export module isshlyapin.render:exceptions;

namespace ogl {

export
class GLFWError : public std::runtime_error {
public:
    explicit GLFWError(const std::string& msg, std::string log)
      : std::runtime_error(msg), log_(std::move(log)) {}

    [[nodiscard]] const std::string& log() const { return log_; }
private:
    std::string log_;
};

export
class OGLError : public std::runtime_error {
public:
    explicit OGLError(const std::string& msg)
      : std::runtime_error(msg) {}
};

export
class OGLInvalidEnum final : public OGLError {
public:
    explicit OGLInvalidEnum(const std::string& msg)
      : OGLError(msg) {}
};

export
class OGLInvalidValue final : public OGLError {
public:
    explicit OGLInvalidValue(const std::string& msg)
      : OGLError(msg) {}
};

export
class OGLInvalidOperation final : public OGLError {
public:
    explicit OGLInvalidOperation(const std::string& msg)
      : OGLError(msg) {}
};

export
class OGLInvalidFramebufferOperation final : public OGLError {
public:
    explicit OGLInvalidFramebufferOperation(const std::string& msg)
      : OGLError(msg) {}
};

export
class OGLOutOfMemory final : public OGLError {
public:
    explicit OGLOutOfMemory(const std::string& msg)
      : OGLError(msg) {}
};

export
class OGLStackUnderflow final : public OGLError {
public:
    explicit OGLStackUnderflow(const std::string& msg)
      : OGLError(msg) {}
};

export
class OGLStackOverflow final : public OGLError {
public:
    explicit OGLStackOverflow(const std::string& msg)
      : OGLError(msg) {}
};

export
class OGLUniformNotFound final : public OGLError {
public:
    explicit OGLUniformNotFound(const std::string& msg)
      : OGLError(msg) {}
};

export
class ShaderError : public std::runtime_error {
public:
    explicit ShaderError(const std::string& msg) 
      : std::runtime_error(msg) {}
};

export
class ShaderCompileError final : public ShaderError {
public:
    explicit ShaderCompileError(const std::string& msg, std::string log)
      : ShaderError(msg), log_(std::move(log)) {}

    [[nodiscard]] const std::string& log() const { return log_; }
private:
    std::string log_;
};

export class ShaderLinkError final : public ShaderError {
public:
    explicit ShaderLinkError(const std::string& msg, std::string log)
      : ShaderError(msg), log_(std::move(log)) {}

    [[nodiscard]] const std::string& log() const { return log_; }
private:    
    std::string log_;
};

} // namespace ogl