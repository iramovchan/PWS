#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <../../../glm/glm/glm.hpp>

class Rigidbody{
public:
    Rigidbody(float mass, glm::vec3 position = glm::vec3(0.0f));

    void applyForce(const glm::vec3& newForce);
    void update(float deltaTime);

private:
    float mass;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 force;
};

#endif