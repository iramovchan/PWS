#pragma once

#ifdef _WIN32
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#elif defined(__APPLE__)
#include <OpenGL/gl3.h>  // macOS OpenGL
#include <../../glm/glm/glm.hpp>
#include <../../glm/glm/gtc/matrix_transform.hpp>
#endif

using aabb_t = std::pair<glm::vec3, glm::vec3>;

class GameObject {
    public:
    uint64_t id() {
        return reinterpret_cast<uint64_t>(this);
    }

    std::function<bool(const GameObject &, glm::vec3 p)> check_collision;

    // virtual aabb_t aabb();

};
