module;

#include <algorithm>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module isshlyapin.render:camera;

import :config;

namespace ogl {

// Defines several possible options for camera movement. 
// Used as abstraction to stay away from window-system specific input methods
enum class CameraMovement : std::uint8_t {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// An abstract camera class that processes input and calculates 
// the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
export 
class Camera {
public:
    Camera() 
      : yaw_(0), pitch_(0), pos_(0), zoom_(CAMERA_ZOOM), world_up_(0, 1, 0),
        z_near_(CAMERA_ZNEAR), z_far_(CAMERA_ZFAR), movement_speed_(CAMERA_SPEED), 
        mouse_sensitivity_(CAMERA_SENSITIVTY) {
        update_orientation();
    }

    Camera(GLfloat yaw, GLfloat pitch, glm::vec3 pos) 
      : yaw_(yaw), pitch_(pitch), pos_(pos), zoom_(CAMERA_ZOOM), world_up_(0, 1, 0),
        z_near_(CAMERA_ZNEAR), z_far_(CAMERA_ZFAR), movement_speed_(CAMERA_SPEED), 
        mouse_sensitivity_(CAMERA_SENSITIVTY) {
        update_orientation();
    }

    Camera(GLfloat yaw, GLfloat pitch, glm::vec3 pos, GLfloat zoom, glm::vec3 world_up, 
           GLfloat z_near, GLfloat z_far, GLfloat movement_speed, GLfloat mouse_sensitivity) 
      : yaw_(yaw), pitch_(pitch), pos_(pos), zoom_(zoom), world_up_(world_up),
        z_near_(z_near), z_far_(z_far), movement_speed_(movement_speed), 
        mouse_sensitivity_(mouse_sensitivity) {
        update_orientation();
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    [[nodiscard]] glm::mat4 get_view_matrix() const {
        return view_matrix_;
    }

    // Processes input received from any keyboard-like input system. 
    // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void process_keyboard(CameraMovement direction, GLfloat deltaTime) {
        const GLfloat velocity = this->movement_speed_ * deltaTime;
        if (direction == CameraMovement::FORWARD) {
            pos_ += front_ * velocity;
        }
        if (direction == CameraMovement::BACKWARD) { 
            pos_ -= front_ * velocity;
        }
        if (direction == CameraMovement::LEFT) {
            pos_ -= right_ * velocity;
        }
        if (direction == CameraMovement::RIGHT) {
            pos_ += right_ * velocity;
        }
    }

    // Processes input received from a mouse input system. 
    // Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) {
        xoffset *= mouse_sensitivity_;
        yoffset *= mouse_sensitivity_;

        yaw_   += xoffset;
        pitch_ += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        pitch_ = std::clamp(pitch_, -89.9f, 89.9f);

        // Update Front, Right and Up Vectors using the updated Eular angles
        update_orientation();
    }

    // Processes input received from a mouse scroll-wheel event. 
    // Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(GLfloat yoffset) {
        zoom_ = std::clamp(zoom_ - yoffset, 1.0f, 45.0f);
    }

    [[nodiscard]] GLfloat get_zoom() const {
        return zoom_;
    }

    [[nodiscard]] float get_z_near() const {
        return z_near_;
    }

    [[nodiscard]] float get_z_far() const {
        return z_far_;
    }
    
    [[nodiscard]] const glm::vec3& get_position() const {
        return pos_;
    }

private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void update_orientation() {
        front_ = {
            cos(glm::radians(yaw_)) * cos(glm::radians(pitch_)),
            sin(glm::radians(pitch_)),
            sin(glm::radians(yaw_)) * cos(glm::radians(pitch_))
        };
        front_ = glm::normalize(front_);
        right_ = glm::normalize(glm::cross(front_, world_up_));
        up_    = glm::normalize(glm::cross(right_, front_));

        view_matrix_ = glm::lookAt(pos_, pos_ + front_, up_);
    }

    // Camera Attributes
    GLfloat yaw_;
    GLfloat pitch_;
    glm::vec3 pos_;

    glm::vec3 front_;
    glm::vec3 right_;
    glm::vec3 up_;
    
    GLfloat zoom_;

    glm::vec3 world_up_;

    // Camera options
    float z_near_;
    float z_far_;
    GLfloat movement_speed_;
    GLfloat mouse_sensitivity_;

    glm::mat4 view_matrix_;
};

} // namespace ogl