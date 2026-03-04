module;

#include <stdexcept>
#include <algorithm>

#include <GLFW/glfw3.h>

export module isshlyapin.render:window;

import :config;
import :helpers;
import :exceptions;

namespace ogl {

constexpr const size_t MAX_KEYS = 1024;

export
struct Keyboard {
    Keyboard() {
        std::ranges::fill(keys, false);
    }

    [[nodiscard]] bool is_pressed(size_t key) const {
        if (key < 0 || key >= MAX_KEYS) {
            throw std::out_of_range("Key code out of range");
        }
        return keys[key];
    }

    void press(size_t key) {
        if (key < 0 || key >= MAX_KEYS) {
            throw std::out_of_range("Key code out of range");
        }
        keys[key] = true;
    }

    void release(size_t key) {
        if (key < 0 || key >= MAX_KEYS) {
            throw std::out_of_range("Key code out of range");
        }
        keys[key] = false;
    }

private:
    std::array<bool, MAX_KEYS> keys{};
};

export
struct Mouse {
    double xpos;
    double ypos;
    double xoffset;
    double yoffset;
    double scroll;
};

export 
struct InputState {
    InputState() : mouse{.xpos=0, .ypos=0, .xoffset=0, .yoffset=0, .scroll=0} {}

    Keyboard keyboard;
    Mouse mouse;
};

export
class Window {
public:
    Window(GLuint width = INIT_SCREEN_WIDTH, GLuint height = INIT_SCREEN_HEIGHT, const char* title = "") 
      : window_(nullptr), width_(width), height_(height), title_(title) {
        try {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_SAMPLES, 4);
            
            window_ = glfwCreateWindow(width_, height_, title, nullptr, nullptr);
            glfwMakeContextCurrent(window_);
    
            glfwSetWindowUserPointer(window_, this);

            glfwSetKeyCallback(window_, key_callback);
            glfwSetScrollCallback(window_, scroll_callback);
            glfwSetCursorPosCallback(window_, mouse_callback);
            glfwSetFramebufferSizeCallback(window_, resize_callback);
        } catch (...) {
            glfwDestroyWindow(window_);
            throw;
        }
    }

    ~Window() {
        glfwDestroyWindow(window_);
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&& other) noexcept : window_(other.window_), width_(other.width_), height_(other.height_), title_(other.title_) {
        other.window_ = nullptr;
    }
    Window& operator=(Window&& other) noexcept {
        if (this != &other) { std::swap(*this, other); }
        return *this;
    }

    [[nodiscard]] GLuint width() const { return width_; }
    [[nodiscard]] GLuint height() const { return height_; }

    InputState& input() { return input_; }
    [[nodiscard]] const InputState& input() const { return input_; }

    [[nodiscard]] bool should_close() const { return glfwWindowShouldClose(window_); }

    void swap_buffers() const { glfwSwapBuffers(window_); }

private:
    static Window* self(GLFWwindow* w) {
        return static_cast<Window*>(glfwGetWindowUserPointer(w));
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        if (action == GLFW_PRESS) {
            self(window)->input_.keyboard.press(key);
        } else if (action == GLFW_RELEASE) {
            self(window)->input_.keyboard.release(key);
        }
    }

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        auto& mouse = self(window)->input_.mouse;
        mouse.xoffset = xpos - mouse.xpos;
        mouse.yoffset = ypos - mouse.ypos;
        mouse.xpos = xpos;
        mouse.ypos = ypos;
    }

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        self(window)->input_.mouse.scroll = yoffset;
    }

    static void resize_callback(GLFWwindow* window, int width, int height) {
        self(window)->width_  = width;
        self(window)->height_ = height;
        glViewport(0, 0, width, height);
    }

    GLFWwindow* window_;
    GLuint width_;
    GLuint height_;
    const char* title_;
    InputState input_;
};

} // namespace ogl