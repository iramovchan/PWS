// implementation of the class

// #include "rigidbody.h"


// // constructor
// RigidBody::RigidBody(float c_mass, glm::vec3 c_position, bool c_is_static) 
// {
//     mass = c_mass;
//     position = c_position;
//     is_static = c_is_static;
//     velocity = glm::vec3(0.0f);
//     acceleration = glm::vec3(0.0f);
//     force = glm::vec3(0.0f);
// }


// void RigidBody::applyForce(const glm::vec3& newForce) {
//     if (!is_static)
//     {
//         acceleration += force / mass; // F = ma
//         // force += newForce;
//     }
//     // gravity other forces bla bla
// }

// // Update physics calculations each frame
// void RigidBody::update(float deltaTime) 
// {
//      if (!is_static) {
//         velocity += acceleration * deltaTime;
//         position += velocity * deltaTime;
//         acceleration = glm::vec3(0.0f); // Reset acceleration for the next frame
//     }
//     // acceleration = force / mass;
//     // velocity += acceleration * deltaTime;
//     // position += velocity * deltaTime;
//     // force = glm::vec3(0.0f);
// }


// void RigidBody::checkCollision(const RigidBody& other)
// {
//     // check if this object collides wiht another
// }

// void RigidBody::resolveCollision(RigidBody& other)
// {
//     // handle collision response
// }