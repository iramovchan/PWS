// class definition and function definition

#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#ifdef _WIN32
#include <glm/glm.hpp>
#elif defined(__APPLE__)
#include <../../../glm/glm/glm.hpp>
#endif

#include "modelCode.h"

class RigidBody{
public:
    float mass;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    bool is_static;
    bool is_animated;
    bool camera_attached;
    bool camera;
    glm::vec3 force = glm::vec3(0.0f);
    float damping; // ????????
    Model model;
    Animator* animator;

    // RigidBody(float mass, glm::vec3 position = glm::vec3(0.0f), bool is_static = false);
    RigidBody(float mass, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, bool is_static, bool is_animated, const Model& modelPath, bool camera, bool camera_attached)
        : mass(mass), position(position), rotation(rotation), scale(scale), is_static(is_static), is_animated(is_animated), model(modelPath), camera(camera), camera_attached(camera_attached) {
        // The model is initialized using its constructor with the path.
    }

    void applyForce(const glm::vec3& force)
    {
        // if (!is_static && (!camera && !camera_attached))
        if (!is_static)
        {
            acceleration += force / mass; // F = ma
            // force += newForce;
        }
        // gravity other forces bla bla
    };
    void update(float deltaTime)
    {
        if (!is_static) 
        {
            velocity += acceleration * deltaTime;
            position += velocity * deltaTime;
            acceleration = glm::vec3(0.0f); // Reset acceleration for the next frame
        }

        // acceleration = force / mass;
        // velocity += acceleration * deltaTime;
        // position += velocity * deltaTime;
        // force = glm::vec3(0.0f);
    };

    // void updateAnimation(Animator* currentAnimator)
    // {
    //     animator = currentAnimator;
    // };
    void checkCollision(const RigidBody& other);
    void resolveCollision(RigidBody& other);

private:

};

#endif

// f = m*a = m * v/t
// p.. = 1/m * f (acceleration is the second derivative of position)
