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
    // bool isBullet;
    std::string tag;
    
    glm::vec3 force = glm::vec3(0.0f);
    float damping; // ????????
    Model* model;
    std::vector<std::pair<glm::vec3, glm::vec3>> worldAABBs; // Min/Max pairs for each mesh
    Animator* animator;

    // RigidBody(float mass, glm::vec3 position = glm::vec3(0.0f), bool is_static = false);
    RigidBody(float mass, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, glm::vec3 velocity, bool is_static, bool is_animated, std::string tag, Model* model)
        : mass(mass), position(position), rotation(rotation), scale(scale), velocity(velocity), is_static(is_static), is_animated(is_animated), tag(tag), model(model) {
        // The model is initialized using its constructor with the path.
        updateWorldAABBs();
        // std::cout << "amount of meshes: " << model->meshes.size() << std::endl;
        // std::cout << "amount of world aabb's" << worldAABBs.size() << std::endl;
        // for (const auto& pair : worldAABBs) {
        //     glm::vec3 minCoords = pair.first;  // First glm::vec3 (min corner)
        //     glm::vec3 maxCoords = pair.second; // Second glm::vec3 (max corner)
            
        //     std::cout << "Min corner: (" 
        //             << minCoords.x << ", " 
        //             << minCoords.y << ", " 
        //             << minCoords.z << ")\n";
            
        //     std::cout << "Max corner: (" 
        //             << maxCoords.x << ", " 
        //             << maxCoords.y << ", " 
        //             << maxCoords.z << ")\n";
        // }
    }

    void updateWorldAABBs() {
        worldAABBs.clear();
        // glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), scale);
        // glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 1, 0)) *
        // glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1, 0, 0)) *
                                
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position); // Only translation
        
        modelMatrix = glm::scale(modelMatrix, scale);       // Only scale


        for (int i = 0; i < model->meshes.size(); ++i) {
            auto& mesh = model->meshes[i];

            // glm::vec3 minWorld = glm::vec3(modelMatrix * glm::vec4(mesh->mAABB.mmin  mesh.aabb.mMin.x, mesh.aabb.mMin.y, mesh.aabb.mMin.z, 1.0f));
            // glm::vec3 maxWorld = glm::vec3(modelMatrix * glm::vec4(mesh.aabb.mMax.x, mesh.aabb.mMax.y, mesh.aabb.mMax.z, 1.0f));
            // glm::vec3 minWorld = glm::vec3(modelMatrix * glm::vec4(mesh.aabb))
            glm::vec3 minWorld = glm::vec3(modelMatrix * glm::vec4(mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z, 1.0f));
            glm::vec3 maxWorld = glm::vec3(modelMatrix * glm::vec4(mesh.mAABB.mMax.x, mesh.mAABB.mMax.y, mesh.mAABB.mMax.z, 1.0f));
            
            // if (tag == "aabbcube") {
            //     // glm::vec3 minWorld = glm::vec3(0.0f, 0.0f, )
            //     std::cout << tag << ", meshmAABBmMin: " << mesh.mAABB.mMin.x << ", " << mesh.mAABB.mMin.y << ", " << mesh.mAABB.mMin.z << std::endl;
            //     std::cout << tag << ", meshmAABBmMax: " << mesh.mAABB.mMax.x << ", " << mesh.mAABB.mMax.y << ", " << mesh.mAABB.mMax.z << std::endl;
            //     std::cout << tag << ", minworld: " << minWorld.x << ", " << minWorld.y << ", " << minWorld.z << std::endl;
            //     std::cout << tag << ", maxworld: " << maxWorld.x << ", " << maxWorld.y << ", " << maxWorld.z << std::endl;
            // }
            
            
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



    bool checkCollision(const std::pair<glm::vec3, glm::vec3>& other) {
        for (const auto& aabbA : worldAABBs) {
            if (checkAABBCollision(aabbA.first, aabbA.second, other.first, other.second)) {
                return true;
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
            if(tag == "bullet") {
                // bullets[i].position += bullets[i].velocity * deltaTime;
                position += velocity * deltaTime;
            }
            // else if (tag == "player") {
            //     std::cout << 
            // }
            else {
                // velocity += acceleration * deltaTime;
                // position += velocity * deltaTime;
                acceleration = glm::vec3(0.0f); // Reset acceleration for the next frame
            }
            
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

    // void resolveAABBCollision(const std::pair<glm::vec3, glm::vec3>& aabbA,
    //                           const std::pair<glm::vec3, glm::vec3>& aabbB,
    //                           RigidBody& other) {
    //     // Basic collision response (separating axes)
    //     glm::vec3 penetration = glm::vec3(0.0f);
    //     if (!is_static) {
    //         penetration = computePenetration(aabbA, aabbB);
    //         position -= penetration; // Move back
    //         velocity = glm::vec3(0.0f); // Reset velocity
    //         // is_static = true;
    //     }
    //     if (!other.is_static) {
    //         penetration = computePenetration(aabbB, aabbA);
    //         other.position += penetration; // Move other back
    //         other.velocity = glm::vec3(0.0f);
    //         // other.is_static = true;
    //     }
    // }

    void resolveAABBCollision(const std::pair<glm::vec3, glm::vec3>& aabbA,
                          const std::pair<glm::vec3, glm::vec3>& aabbB,
                          RigidBody& other) {
        glm::vec3 penetration = computePenetration(aabbA, aabbB);

        if (!is_static && other.is_static) { 
            // If this object moves but the other is static (like walls/floor)
            position -= penetration; 
        } 
        else if (is_static && !other.is_static) { 
            // If the other object moves but this one is static
            other.position += penetration;
        } 
        else if (!is_static && !other.is_static) { 
            // If both objects can move, split penetration evenly
            position -= penetration * 0.5f;
            other.position += penetration * 0.5f;
        }

        // Stop movement in that direction
        velocity = glm::vec3(0.0f);
        other.velocity = glm::vec3(0.0f);
    }


    // glm::vec3 computePenetration(const std::pair<glm::vec3, glm::vec3>& aabbA,
    //                              const std::pair<glm::vec3, glm::vec3>& aabbB) {
    //     glm::vec3 overlapMin = glm::max(aabbA.first, aabbB.first);
    //     glm::vec3 overlapMax = glm::min(aabbA.second, aabbB.second);
    //     return overlapMax - overlapMin; // Penetration depth
    // }

    glm::vec3 computePenetration(const std::pair<glm::vec3, glm::vec3>& aabbA,
                             const std::pair<glm::vec3, glm::vec3>& aabbB) {
        glm::vec3 penetration = glm::vec3(0.0f);

        float overlapX = std::min(aabbA.second.x, aabbB.second.x) - std::max(aabbA.first.x, aabbB.first.x);
        float overlapY = std::min(aabbA.second.y, aabbB.second.y) - std::max(aabbA.first.y, aabbB.first.y);
        float overlapZ = std::min(aabbA.second.z, aabbB.second.z) - std::max(aabbA.first.z, aabbB.first.z);

        if (overlapX < overlapY && overlapX < overlapZ) {
            penetration.x = overlapX;
        } else if (overlapY < overlapZ) {
            penetration.y = overlapY;
        } else {
            penetration.z = overlapZ;
        }

        return penetration;
    }


};

#endif
