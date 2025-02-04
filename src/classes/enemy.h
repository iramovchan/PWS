#ifndef ENEMY_H
#define ENEMY_H

#ifdef _WIN32
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#elif defined(__APPLE__)
#include <OpenGL/gl3.h>  // macOS OpenGL
#include <../../glm/glm/glm.hpp>
#include <../../glm/glm/gtc/matrix_transform.hpp>
#endif

#include "shaderCode_s.h"

#include "game_object.h"

class Enemy : public GameObject {
public:
    glm::vec3 position;   // Position in world space
    glm::vec3 velocity;   // Direction and speed
    glm::vec3 direction;  // Current movement direction
    float speed;          // Movement speed

    Enemy(glm::vec3 initialPos) {
        position = initialPos;
        velocity = glm::vec3(0.0f);
        direction = glm::vec3(1.0f, 0.0f, 0.0f); // Initially moving along the X axis
        speed = 0.01f; // Movement speed
    }

    std::function<bool(const glm::vec3 &)> check_collision;

    void update() {
        // Move the enemy
        glm::vec3 nextPos = position;
        bool collides = false;
        nextPos += direction * speed;
        if(check_collision) {
            collides = check_collision(nextPos);
        }
        if(!collides) {
            position = nextPos;
        }
        // position += direction * speed;
        

        // If the enemy hits an obstacle, change direction
        // if (detectCollision()) {
        //     changeDirection();
        // }
    }



    // Change the enemy's direction
    void changeDirection() {
        // Randomly pick a new direction
        float angle = rand() % 360; // Random angle
        direction.x = cos(glm::radians(angle));
        direction.z = sin(glm::radians(angle));
    }

};

#endif