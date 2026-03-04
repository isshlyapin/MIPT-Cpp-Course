module;

#include <GL/glew.h>
#include <glm/glm.hpp>

export module isshlyapin.render:render;

import :scene;
import :config;
import :camera;
import :window;
import :helpers;


namespace ogl {

export class Renderer {
public:
    explicit Renderer(glm::vec3 clear_color = BACKGROUND_COLOR)
      : clear_color_(clear_color) {}

    void begin_frame() const {
        glClearColor(clear_color_.r, clear_color_.g, clear_color_.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void render(Scene& scene, Camera& camera, Window& window) const {
        for (const auto& obj : scene.objects) {
            obj.program->use();
            const auto light_color_loc = obj.program->get_uniform_location("light_color");
            const auto light_pos_loc = obj.program->get_uniform_location("light_pos");

            const auto view_pos_loc = obj.program->get_uniform_location("view_pos");

            const auto view_loc = obj.program->get_uniform_location("view");
            const auto projection_loc = obj.program->get_uniform_location("projection");

            const glm::mat4 projection = glm::perspective(
                camera.get_zoom(), (float)window.width()/(float)window.height(), 
                camera.get_z_near(), camera.get_z_far()
            );

            gl_call(glUniform3fv, light_color_loc, 1, glm::value_ptr(scene.light.color));
            gl_call(glUniform3fv, light_pos_loc, 1, glm::value_ptr(scene.light.position));
            gl_call(glUniform3fv, view_pos_loc, 1, glm::value_ptr(camera.get_position()));

            gl_call(glUniformMatrix4fv, view_loc, 1, GL_FALSE, glm::value_ptr(camera.get_view_matrix()));
            gl_call(glUniformMatrix4fv, projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

            obj.mesh->draw();
        }
    }

private:
    glm::vec3 clear_color_;    
};

} // namespace ogl
