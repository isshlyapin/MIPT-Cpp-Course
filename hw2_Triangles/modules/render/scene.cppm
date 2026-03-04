module;

#include <vector>

#include <glm/glm.hpp>

export module isshlyapin.render:scene;

import :mesh;

namespace ogl {

export
struct Light {
    glm::vec3 color;
    glm::vec3 position;
};

export
struct Scene {
    Light light;
    std::vector<Renderable> objects;
};

} // namespace ogl