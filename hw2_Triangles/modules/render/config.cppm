module;

#include <GL/glew.h>
#include <glm/glm.hpp>

export module isshlyapin.render:config;

namespace ogl {

// export constexpr const char* OGL_APP_NAME = "TriangleIntersectionRenderer";

export constexpr const glm::vec3 BACKGROUND_COLOR = {0.463f, 0.51f, 0.459f};

export constexpr const GLuint INIT_SCREEN_WIDTH  = 800;
export constexpr const GLuint INIT_SCREEN_HEIGHT = 600;

export constexpr const GLfloat CAMERA_ZOOM =  45.0f;

export constexpr const GLfloat CAMERA_SPEED      =  3.0f;
export constexpr const GLfloat CAMERA_SENSITIVTY =  0.25f;

export constexpr const GLfloat CAMERA_ZFAR  =  1000.0f;
export constexpr const GLfloat CAMERA_ZNEAR =  0.1f;


export constexpr const GLchar* VERTEX_SHADER_PATH = "modules/render/vertex_shader.glsl";
export constexpr const GLchar* FRAGMENT_SHADER_PATH = "modules/render/fragment_shader.glsl";

} // namespace ogl
