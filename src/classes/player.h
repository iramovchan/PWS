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
   
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Player(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updatePlayerVectors();
    }
    
    // constructor with scalar values
    Player(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updatePlayerVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Player_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD_P)
            Position += glm::vec3(Front.x, 0.0f, Front.z) * velocity;  // Limit to x and z axis
        if (direction == BACKWARD_P)
            Position -= glm::vec3(Front.x, 0.0f, Front.z) * velocity; // Limit to x and z axis
        if (direction == LEFT_P)
            Position -= glm::vec3(Right.x, 0.0f, Right.z) * velocity; // Limit to x and z axis
        if (direction == RIGHT_P)
            Position += glm::vec3(Right.x, 0.0f, Right.z) * velocity;  // Limit to x and z axis
        
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updatePlayerVectors();
    }



private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updatePlayerVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};


#endif