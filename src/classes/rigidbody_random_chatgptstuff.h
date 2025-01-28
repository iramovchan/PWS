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
    
    glm::vec3 force = glm::vec3(0.0f);
    float damping; // ????????
    Model* model;
    std::vector<std::pair<glm::vec3, glm::vec3>> worldAABBs; // Min/Max pairs for each mesh
    Animator* animator;

    // RigidBody(float mass, glm::vec3 position = glm::vec3(0.0f), bool is_static = false);
    RigidBody(float mass, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, bool is_static, bool is_animated, Model* model)
        : mass(mass), position(position), rotation(rotation), scale(scale), is_static(is_static), is_animated(is_animated), model(model) {
        // The model is initialized using its constructor with the path.
        updateWorldAABBs();
    }

    void updateWorldAABBs() {
        worldAABBs.clear();
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
        
        for (int i = 0; i < model->meshes.size(); ++i) {
            auto& mesh = model->meshes[i];

            glm::vec3 minWorld = glm::vec3(modelMatrix * glm::vec4(mesh.aabb.mMin.x, mesh.aabb.mMin.y, mesh.aabb.mMin.z, 1.0f));
            glm::vec3 maxWorld = glm::vec3(modelMatrix * glm::vec4(mesh.aabb.mMax.x, mesh.aabb.mMax.y, mesh.aabb.mMax.z, 1.0f));
            worldAABBs.emplace_back(minWorld, maxWorld);
        }
    }

    bool checkCollision(const RigidBody& other) {
        for (const auto& aabbA : worldAABBs) {
            for (const auto& aabbB : other.worldAABBs) {
                if (checkAABBCollision(aabbA.first, aabbA.second, aabbB.first, aabbB.second)) {
                    return true;
                }
            }
        }
        return false;
    }

    void resolveCollision(RigidBody& other) {
        for (size_t i = 0; i < worldAABBs.size(); ++i) {
            for (size_t j = 0; j < other.worldAABBs.size(); ++j) {
                const auto& aabbA = worldAABBs[i];
                const auto& aabbB = other.worldAABBs[j];
                if (checkAABBCollision(aabbA.first, aabbA.second, aabbB.first, aabbB.second)) {
                    resolveAABBCollision(aabbA, aabbB, other);
                }
            }
        }
    }

    void applyForce(const glm::vec3& force)
    {
       
        if (!is_static)
        {
            acceleration += force / mass; // F = ma
            // force += newForce;
        }
        
    };
    void update(float deltaTime)
    {
        if (!is_static) 
        {
            velocity += acceleration * deltaTime;
            position += velocity * deltaTime;
            acceleration = glm::vec3(0.0f); // Reset acceleration for the next frame
        }
        updateWorldAABBs();

        // acceleration = force / mass;
        // velocity += acceleration * deltaTime;
        // position += velocity * deltaTime;
        // force = glm::vec3(0.0f);
    };

    // void updateAnimation(Animator* currentAnimator)
    // {
    //     animator = currentAnimator;
    // };
    // bool IsColliding(const RigidBody& other){
    //     glm::vec3 aMin = position - scale / 2.0f;
    //     glm::vec3 aMax = position + scale / 2.0f;

    //     glm::vec3 bMin = other.position - other.scale / 2.0f;
    //     glm::vec3 bMax = other.position + other.scale / 2.0f;

    //     return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y) && (aMin.z <= bMax.z && aMax.z >= bMin.z);
    // };
    // // void resolveCollision(RigidBody& other);

    // void resolveAABBCollision(RigidBody& other) {
    // // Only resolve if there's a collision
    //     if (!IsColliding(other)) return;

    //     glm::vec3 aMin = position - scale / 2.0f;
    //     glm::vec3 aMax = position + scale / 2.0f;

    //     glm::vec3 bMin = other.position - other.scale / 2.0f;
    //     glm::vec3 bMax = other.position + other.scale / 2.0f;

    //     // Calculate overlap on each axis
    //     float overlapX = std::min(aMax.x - bMin.x, bMax.x - aMin.x);
    //     float overlapY = std::min(aMax.y - bMin.y, bMax.y - aMin.y);
    //     float overlapZ = std::min(aMax.z - bMin.z, bMax.z - aMin.z);

    //     // Determine the smallest overlap axis (axis of least penetration)
    //     if (overlapX < overlapY && overlapX < overlapZ) {
    //         // Resolve along the X-axis
    //         if (position.x < other.position.x) {
    //             position.x -= overlapX / 2.0f;
    //             other.position.x += overlapX / 2.0f;
    //         } else {
    //             position.x += overlapX / 2.0f;
    //             other.position.x -= overlapX / 2.0f;
    //         }
    //     } else if (overlapY < overlapZ) {
    //         // Resolve along the Y-axis
    //         if (position.y < other.position.y) {
    //             position.y -= overlapY / 2.0f;
    //             other.position.y += overlapY / 2.0f;
    //         } else {
    //             position.y += overlapY / 2.0f;
    //             other.position.y -= overlapY / 2.0f;
    //         }
    //     } else {
    //         // Resolve along the Z-axis
    //         if (position.z < other.position.z) {
    //             position.z -= overlapZ / 2.0f;
    //             other.position.z += overlapZ / 2.0f;
    //         } else {
    //             position.z += overlapZ / 2.0f;
    //             other.position.z -= overlapZ / 2.0f;
    //         }
    //     }

    //     // Adjust velocities to simulate collision response (basic elastic collision)
    //     glm::vec3 normal = glm::normalize(other.position - position);
    //     glm::vec3 relativeVelocity = velocity - other.velocity;
    //     float velocityAlongNormal = glm::dot(relativeVelocity, normal);

    //     if (velocityAlongNormal > 0.0f) return;  // Objects are moving apart

    //     float e = 1.0f;  // Coefficient of restitution (1.0 = perfectly elastic)
    //     float j = -(1 + e) * velocityAlongNormal / (1 / mass + 1 / other.mass);

    //     glm::vec3 impulse = j * normal;
    //     velocity -= impulse / mass;
    //     other.velocity += impulse / other.mass;
    // };


private:
    bool checkAABBCollision(const glm::vec3& minA, const glm::vec3& maxA,
                            const glm::vec3& minB, const glm::vec3& maxB) {
        return (minA.x <= maxB.x && maxA.x >= minB.x) &&
               (minA.y <= maxB.y && maxA.y >= minB.y) &&
               (minA.z <= maxB.z && maxA.z >= minB.z);
    }

    void resolveAABBCollision(const std::pair<glm::vec3, glm::vec3>& aabbA,
                              const std::pair<glm::vec3, glm::vec3>& aabbB,
                              RigidBody& other) {
        // Basic collision response (separating axes)
        glm::vec3 penetration = glm::vec3(0.0f);
        if (!is_static) {
            penetration = computePenetration(aabbA, aabbB);
            position -= penetration; // Move back
            velocity = glm::vec3(0.0f); // Reset velocity
        }
        if (!other.is_static) {
            penetration = computePenetration(aabbB, aabbA);
            other.position += penetration; // Move other back
            other.velocity = glm::vec3(0.0f);
        }
    }

    glm::vec3 computePenetration(const std::pair<glm::vec3, glm::vec3>& aabbA,
                                 const std::pair<glm::vec3, glm::vec3>& aabbB) {
        glm::vec3 overlapMin = glm::max(aabbA.first, aabbB.first);
        glm::vec3 overlapMax = glm::min(aabbA.second, aabbB.second);
        return overlapMax - overlapMin; // Penetration depth
    }

};

    // bool checkAABBCollision(const RigidBody& other) const {
    //     glm::vec3 aMin = position - scale / 2.0f;
    //     glm::vec3 aMax = position + scale / 2.0f;

    //     glm::vec3 bMin = other.position - other.scale / 2.0f;
    //     glm::vec3 bMax = other.position + other.scale / 2.0f;

    //     return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
    //         (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
    //         (aMin.z <= bMax.z && aMax.z >= bMin.z);
    // }
















/*
FOR chatgpt

ok so right now for collisions it kind of works, at least it correctly identifies the collision detection, only the response isn't quite working correctly:


bool checkCollision(const RigidBody& other) {
        for (const auto& aabbA : worldAABBs) {
            for (const auto& aabbB : other.worldAABBs) {
                if (checkAABBCollision(aabbA.first, aabbA.second, aabbB.first, aabbB.second)) {
                    return true;
                }
            }
        }
        return false;
    }


    void resolveCollision(RigidBody& other) {
        for (size_t i = 0; i < worldAABBs.size(); ++i) {
            for (size_t j = 0; j < other.worldAABBs.size(); ++j) {
                const auto& aabbA = worldAABBs[i];
                const auto& aabbB = other.worldAABBs[j];
                if (checkAABBCollision(aabbA.first, aabbA.second, aabbB.first, aabbB.second)) {
                    resolveAABBCollision(aabbA, aabbB, other);
                }
            }
        }
    }


    void applyForce(const glm::vec3& force)
    {
       
        if (!is_static)
        {
            acceleration += force / mass; // F = ma
            // force += newForce;
        }
        
    };
    void update(float deltaTime)
    {
        if (!is_static) 
        {
            velocity += acceleration * deltaTime;
            position += velocity * deltaTime;
            acceleration = glm::vec3(0.0f); // Reset acceleration for the next frame
        }
        updateWorldAABBs();

        // acceleration = force / mass;
        // velocity += acceleration * deltaTime;
        // position += velocity * deltaTime;
        // force = glm::vec3(0.0f);
    };




private:
    bool checkAABBCollision(const glm::vec3& minA, const glm::vec3& maxA,
                            const glm::vec3& minB, const glm::vec3& maxB) {
        return (minA.x <= maxB.x && maxA.x >= minB.x) &&
               (minA.y <= maxB.y && maxA.y >= minB.y) &&
               (minA.z <= maxB.z && maxA.z >= minB.z);
    }

    void resolveAABBCollision(const std::pair<glm::vec3, glm::vec3>& aabbA,
                              const std::pair<glm::vec3, glm::vec3>& aabbB,
                              RigidBody& other) {
        // Basic collision response (separating axes)
        glm::vec3 penetration = glm::vec3(0.0f);
        if (!is_static) {
            penetration = computePenetration(aabbA, aabbB);
            position -= penetration; // Move back
            velocity = glm::vec3(0.0f); // Reset velocity
            is_static = true;
        }
        if (!other.is_static) {
            penetration = computePenetration(aabbB, aabbA);
            other.position += penetration; // Move other back
            other.velocity = glm::vec3(0.0f);
            other.is_static = true;
        }
    }

    glm::vec3 computePenetration(const std::pair<glm::vec3, glm::vec3>& aabbA,
                                 const std::pair<glm::vec3, glm::vec3>& aabbB) {
        glm::vec3 overlapMin = glm::max(aabbA.first, aabbB.first);
        glm::vec3 overlapMax = glm::min(aabbA.second, aabbB.second);
        return overlapMax - overlapMin; // Penetration depth
    }

};

#endif

i added the things to make the objects static on collision because when collision happened the object just flew some random direction and then completely disappeared, now on collision it stops on the ground and doesn't move any more but it somehow jerks a bit to the side for some reason??? and it happens with all the objects and i dont understand why

*/