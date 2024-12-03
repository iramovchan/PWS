#include "rigidbody.h"


// constructor
Rigidbody::Rigidbody(float mass, glm::vec3 position)
    : mass(mass), position(position), velocity(glm::vec3(0.0f)), 
      acceleration(glm::vec3(0.0f)), force(glm::vec3(0.0f)) {}

void Rigidbody::applyForce(const glm::vec3& newForce) {
    force += newForce;
}

// Update physics calculations each frame
void Rigidbody::update(float deltaTime) {
    acceleration = force / mass;
    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;
    force = glm::vec3(0.0f);
}
