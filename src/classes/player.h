#ifndef PLAYER
#define PLAYER

#ifdef _WIN32
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#elif defined(__APPLE__)
#include <OpenGL/gl3.h>  // macOS OpenGL
#include <../../glm/glm/glm.hpp>
#include <../../glm/glm/gtc/matrix_transform.hpp>
#endif

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Player_Movement {
    FORWARD_P,
    BACKWARD_P,
    LEFT_P,
    RIGHT_P,
    // UP,      // Added UP movement
    // DOWN     // Added DOWN movement
};

// Default camera values
// const float YAW_P           = -90.0f;
// const float PITCH_P         =  -30.0f;
// const float SPEED_P         =  2.5f;
// const float SENSITIVITY_P   =  0.1f;
// const float ZOOM_P          =  45.0f;

class Player 
{
public:
   
    // glm::vec3 Position;
    // glm::vec3 Front;
    // glm::vec3 Up;
    // glm::vec3 Right;
    // glm::vec3 WorldUp;
    // // euler Angles
    // float Yaw;
    // float Pitch;
    // // camera options
    // float MovementSpeed;
    // float MouseSensitivity;
    // float Zoom;

    void drawPlayerBoundingBox(Shader& shader, glm::vec3 playerPosition, glm::vec3 playerSize) {
        // glm::vec3 aabbMin = (playerPosition - playerSize) / 2.0f;
        // glm::vec3 aabbMax = (playerPosition + playerSize) / 2.0f;

        glm::vec3 aabbMin = playerPosition - playerSize / 2.0f;
        glm::vec3 aabbMax = playerPosition + playerSize / 2.0f;

        // glm::vec3 aabbMin = glm::vec3(0.0f, 0.0f, 0.0f);
        // forward/backward | up/down | left/right

        /*
        AABB Min Position: (-0.5, 0, -0.5)
        AABB Max Position: (0.5, 2, 0.5)
        Player positions (x, y, z)0 / 1 / 0

        AABB Min Position: (-0.5, -0.5, -0.5)
        AABB Max Position: (0.5, 1.5, 0.5)
        Player positions (x, y, z)0 / 1 / 0

        AABB Min Position: (-0.25, 0, -0.25)
        AABB Max Position: (0.25, 1, 0.25)
        Player positions (x, y, z)0 / 1 / 0
        */

        // glm::vec3 aabbMax = glm::vec3(5.0f, 5.0f, 5.0f);
        std::cout << "AABB Min Position: (" << aabbMin.x << ", " << aabbMin.y << ", " << aabbMin.z << ")" << std::endl;
        std::cout << "AABB Max Position: (" << aabbMax.x << ", " << aabbMax.y << ", " << aabbMax.z << ")" << std::endl;
        std::cout << "Player positions (x, y, z)" << playerPosition.x << " / " << playerPosition.y << " / " << playerPosition.z << std::endl;

        // Model loaded successfully: ../src/models_animated/FPS_arms_animated/FPS_arms_idle.fbx
        // FROM THE MESHCODE: AABBMIN: -1.02045, -0.125138, -0.0560648
        // FROM THE MESHCODE: AABBMAX: 1.02045, 0.0731141, 0.0560648

        // std::cout << "x: " << aabbMax.x << ", y: " << aabbMax.y << ", z: " << aabbMax.z << std::endl;

        // Define the 8 corners of the AABB
        glm::vec3 aabbVertices[8] = {
            {aabbMin.x, aabbMin.y, aabbMin.z},
            {aabbMax.x, aabbMin.y, aabbMin.z},
            {aabbMax.x, aabbMax.y, aabbMin.z},
            {aabbMin.x, aabbMax.y, aabbMin.z},
            {aabbMin.x, aabbMin.y, aabbMax.z},
            {aabbMax.x, aabbMin.y, aabbMax.z},
            {aabbMax.x, aabbMax.y, aabbMax.z},
            {aabbMin.x, aabbMax.y, aabbMax.z}
        };

        // Define the indices for the lines that form the edges of the bounding box
        unsigned int aabbIndices[] = {
            0, 1, 1, 2, 2, 3, 3, 0,  // Bottom face
            4, 5, 5, 6, 6, 7, 7, 4,  // Top face
            0, 4, 1, 5, 2, 6, 3, 7   // Vertical edges
        };

        // Create VAO, VBO, and EBO for the AABB (bounding box)
        GLuint aabbVAO, aabbVBO, aabbEBO;
        glGenVertexArrays(1, &aabbVAO);
        glGenBuffers(1, &aabbVBO);
        glGenBuffers(1, &aabbEBO);

        glBindVertexArray(aabbVAO);

        // VBO for vertices
        glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(aabbVertices), aabbVertices, GL_STATIC_DRAW);

        // EBO for indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabbEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(aabbIndices), aabbIndices, GL_STATIC_DRAW);

        // Set up vertex attribute pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glBindVertexArray(0);  // Unbind VAO

        // Render the AABB
        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, playerPosition); // Use the player position
        model = glm::scale(model, playerSize); // Use player size for scaling
        shader.setMat4("model", model);

        glBindVertexArray(aabbVAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);  // Draw the AABB
        glBindVertexArray(0);
    }

    // // constructor with vectors
    // Player(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    // {
    //     Position = position;
    //     WorldUp = up;
    //     Yaw = yaw;
    //     Pitch = pitch;
    //     updatePlayerVectors();
    // }
    
    // // constructor with scalar values
    // Player(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    // {
    //     Position = glm::vec3(posX, posY, posZ);
    //     WorldUp = glm::vec3(upX, upY, upZ);
    //     Yaw = yaw;
    //     Pitch = pitch;
    //     updatePlayerVectors();
    // }

    // // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    // glm::mat4 GetViewMatrix()
    // {
    //     return glm::lookAt(Position, Position + Front, Up);
    // }

    // // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    // void ProcessKeyboard(Player_Movement direction, float deltaTime)
    // {
    //     float velocity = MovementSpeed * deltaTime;
    //     if (direction == FORWARD_P)
    //         Position += glm::vec3(Front.x, 0.0f, Front.z) * velocity;  // Limit to x and z axis
    //     if (direction == BACKWARD_P)
    //         Position -= glm::vec3(Front.x, 0.0f, Front.z) * velocity; // Limit to x and z axis
    //     if (direction == LEFT_P)
    //         Position -= glm::vec3(Right.x, 0.0f, Right.z) * velocity; // Limit to x and z axis
    //     if (direction == RIGHT_P)
    //         Position += glm::vec3(Right.x, 0.0f, Right.z) * velocity;  // Limit to x and z axis
        
    // }

    // // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    // void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    // {
    //     xoffset *= MouseSensitivity;
    //     yoffset *= MouseSensitivity;

    //     Yaw   += xoffset;
    //     Pitch += yoffset;

    //     // make sure that when pitch is out of bounds, screen doesn't get flipped
    //     if (constrainPitch)
    //     {
    //         if (Pitch > 89.0f)
    //             Pitch = 89.0f;
    //         if (Pitch < -89.0f)
    //             Pitch = -89.0f;
    //     }

    //     // update Front, Right and Up Vectors using the updated Euler angles
    //     updatePlayerVectors();
    // }



private:
    // // calculates the front vector from the Camera's (updated) Euler Angles
    // void updatePlayerVectors()
    // {
    //     // calculate the new Front vector
    //     glm::vec3 front;
    //     front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    //     front.y = sin(glm::radians(Pitch));
    //     front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    //     Front = glm::normalize(front);
    //     // also re-calculate the Right and Up vector
    //     Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    //     Up    = glm::normalize(glm::cross(Right, Front));
    // }
};


#endif