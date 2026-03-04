module;

#include <span>
#include <string>
#include <cstddef>
#include <optional>
#include <stdexcept>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

export 
module isshlyapin.render:triangle_intersection_view;

import :config;
import :camera;
import :shader_source;
import :shader_program;

export 
class TriangleIntersectionRenderer {
public:
    explicit TriangleIntersectionRenderer(const char* name = OGL_APP_NAME, const size_t screen_width = INIT_SCREEN_WIDTH, 
                                          const size_t screen_height = INIT_SCREEN_HEIGHT) 
      : window_(nullptr),
        screen_width_(screen_width), screen_height_(screen_height),
        name_(name) {
        glfw_init();
        window_init();
        set_callbacks();
        set_glfw_options();
        glew_init();

        program_.emplace();
        if (!program_) {
            throw std::runtime_error("Failed to create shader program");
        }

        glfwSetWindowUserPointer(window_, this);

        //TODO Определить относится ли к glew_init или выделить иную функцию
        // Define the viewport dimensions
        glViewport(
            0, 0, 
            static_cast<int>(screen_width_),
            static_cast<int>(screen_height_)
        );

        // Setup some OpenGL options
        glEnable(GL_DEPTH_TEST);

        glfwGetCursorPos(window_, &mouse_xpos_, &mouse_ypos_);
    }

    ~TriangleIntersectionRenderer() {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &position_vbo_);
        glDeleteBuffers(1, &flag_vbo_);
        glfwTerminate();
    }

    void set_buffers(std::span<const float> vertexes, std::span<const float> flags);

    void run();    
private:
    void glfw_init();
    void window_init();
    void set_callbacks();
    void set_glfw_options();
    void glew_init();

    static TriangleIntersectionRenderer* get_app(GLFWwindow* window) {
        return static_cast<TriangleIntersectionRenderer*>(
            glfwGetWindowUserPointer(window)
        );
    }

    static void st_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
        get_app(window)->key_callback(key, scancode, action, mode);
    }
    static void st_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        get_app(window)->mouse_callback(xpos, ypos);
    }
    static void st_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        get_app(window)->scroll_callback(xoffset, yoffset);
    }

    void key_callback(int key, int scancode, int action, int mode);
    void mouse_callback(double xpos, double ypos);
    void scroll_callback(double xoffset, double yoffset);

    void do_movement(GLfloat deltaTime);

    GLFWwindow* window_;
    Camera camera_;
    std::optional<ShaderProgram> program_;
    GLuint position_vbo_;
    GLuint flag_vbo_;
    GLuint vao_;
    GLuint screen_width_;
    GLuint screen_height_;
    std::string name_;
    std::array<bool, 1024> keys_{};
    double mouse_xpos_;
    double mouse_ypos_;
    GLfloat last_frame_;
    size_t count_;
};

void TriangleIntersectionRenderer::glfw_init() {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //TODO Add the ability to resize
    glfwWindowHint(GLFW_SAMPLES, 4);        
}

void TriangleIntersectionRenderer::window_init() {
    window_ = glfwCreateWindow(screen_width_, screen_height_, name_.c_str(), nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window_);    
}

void TriangleIntersectionRenderer::set_callbacks() {
    glfwSetKeyCallback(window_, st_key_callback);
    glfwSetScrollCallback(window_, st_scroll_callback);
    glfwSetCursorPosCallback(window_, st_mouse_callback);
}

void TriangleIntersectionRenderer::set_glfw_options() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void TriangleIntersectionRenderer::glew_init() {
    glewExperimental = GL_TRUE;
    glewInit();
}

// Moves/alters the camera positions based on user input
void TriangleIntersectionRenderer::do_movement(GLfloat deltaTime) {
    // Camera controls
    if(keys_[GLFW_KEY_W])
        camera_.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    if(keys_[GLFW_KEY_S])
        camera_.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
    if(keys_[GLFW_KEY_A])
        camera_.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    if(keys_[GLFW_KEY_D])
        camera_.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
}


// Is called whenever a key is pressed/released via GLFW
void TriangleIntersectionRenderer::key_callback(int key, int scancode, int action, int mode) {
    //cout << key << endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, GL_TRUE);
    }
    if (key >= 0 && key < 1024) {
        if(action == GLFW_PRESS) { 
            keys_[key] = true; 
        } else if(action == GLFW_RELEASE) {
            keys_[key] = false;	
        }
    }
}

void TriangleIntersectionRenderer::mouse_callback(double xpos, double ypos) {
    const auto xoffset = static_cast<GLfloat>(xpos - mouse_xpos_);
    const auto yoffset = static_cast<GLfloat>(mouse_ypos_ - ypos);  // Reversed since y-coordinates go from bottom to left
    
    mouse_xpos_ = xpos;
    mouse_ypos_ = ypos;

    camera_.ProcessMouseMovement(xoffset, yoffset);
}	

void TriangleIntersectionRenderer::scroll_callback(double xoffset, double yoffset) {
    camera_.ProcessMouseScroll(static_cast<GLfloat>(yoffset));
}

void TriangleIntersectionRenderer::set_buffers(std::span<const float> coords, std::span<const float> flags) {
    count_ = coords.size() / 3;  // Количество вершин (3 float на вершину)
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &flag_vbo_);
    glGenBuffers(1, &position_vbo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, position_vbo_);
    glBufferData(GL_ARRAY_BUFFER, coords.size_bytes(), coords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, flag_vbo_);
    glBufferData(GL_ARRAY_BUFFER, flags.size_bytes(), flags.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TriangleIntersectionRenderer::run() {
    while(!glfwWindowShouldClose(window_)) {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        GLfloat deltaTime = currentFrame - last_frame_;
        last_frame_ = currentFrame;

        // Check and call events
        glfwPollEvents();
        do_movement(deltaTime);

        // Clear the colorbuffer
        glClearColor(0.463f, 0.51f, 0.459f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw our first triangle
        program_->Use();

        // Create camera transformation
        glm::mat4 view;
        view = camera_.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(camera_.Zoom, (float)screen_width_/(float)screen_height_, 0.1f, 1000.0f);
        // Get the uniform locations
        // GLint modelLoc = glGetUniformLocation(program_->Program, "model");
        GLint viewLoc = glGetUniformLocation(program_->Program, "view");
        GLint projLoc = glGetUniformLocation(program_->Program, "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, count_);
        glBindVertexArray(0);

        // Swap the buffers
        glfwSwapBuffers(window_);
    }    
}