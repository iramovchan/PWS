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
        std::cout << "amount of meshes: " << model->meshes.size() << std::endl;
        std::cout << "amount of world aabb's" << worldAABBs.size() << std::endl;
        for (const auto& pair : worldAABBs) {
            glm::vec3 minCoords = pair.first;  // First glm::vec3 (min corner)
            glm::vec3 maxCoords = pair.second; // Second glm::vec3 (max corner)
            
            std::cout << "Min corner: (" 
                    << minCoords.x << ", " 
                    << minCoords.y << ", " 
                    << minCoords.z << ")\n";
            
            std::cout << "Max corner: (" 
                    << maxCoords.x << ", " 
                    << maxCoords.y << ", " 
                    << maxCoords.z << ")\n";
        }
    }

    void updateWorldAABBs() {
        worldAABBs.clear();
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
        
        for (int i = 0; i < model->meshes.size(); ++i) {
            auto& mesh = model->meshes[i];

            // glm::vec3 minWorld = glm::vec3(modelMatrix * glm::vec4(mesh->mAABB.mmin  mesh.aabb.mMin.x, mesh.aabb.mMin.y, mesh.aabb.mMin.z, 1.0f));
            // glm::vec3 maxWorld = glm::vec3(modelMatrix * glm::vec4(mesh.aabb.mMax.x, mesh.aabb.mMax.y, mesh.aabb.mMax.z, 1.0f));
            // glm::vec3 minWorld = glm::vec3(modelMatrix * glm::vec4(mesh.aabb))
            glm::vec3 minWorld = glm::vec3(modelMatrix * glm::vec4(mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z, 1.0f));
            glm::vec3 maxWorld = glm::vec3(modelMatrix * glm::vec4(mesh.mAABB.mMax.x, mesh.mAABB.mMax.y, mesh.mAABB.mMax.z, 1.0f));
            
            worldAABBs.emplace_back(minWorld, maxWorld);
        }
    }

    // collision detection

    // void collisionDetection() {
    //     // for each mesh in an .obj we have worldAABBs ig
    //     // check each mesh's aabb's with other object's meshes aabb's
    //     // 
        
    // }

    // collision response

    // collision penetration - how much to transform the object 'back'


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
            // is_static = true;
        }
        if (!other.is_static) {
            penetration = computePenetration(aabbB, aabbA);
            other.position += penetration; // Move other back
            other.velocity = glm::vec3(0.0f);
            // other.is_static = true;
        }

        // if (penetration.y > 0.0f) {
        //     // Assume this is a ground collision, so make static:
        //     is_static = true;
        // }
        // if (penetration.y > 0.0f && otherPenetration.y > 0.0f) {
        //     other.is_static = true;
        // }
    }

    glm::vec3 computePenetration(const std::pair<glm::vec3, glm::vec3>& aabbA,
                                 const std::pair<glm::vec3, glm::vec3>& aabbB) {
        glm::vec3 overlapMin = glm::max(aabbA.first, aabbB.first);
        glm::vec3 overlapMax = glm::min(aabbA.second, aabbB.second);
        return overlapMax - overlapMin; // Penetration depth
    }

};

#endif

// f = m*a = m * v/t
// p.. = 1/m * f (acceleration is the second derivative of position)
