#ifdef _WIN32
#include <glad/glad.h>  // GLAD for Windows
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#elif defined(__APPLE__)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>  // macOS OpenGL
#include <../../glm/glm/glm.hpp>
#include <../../glm/glm/gtc/matrix_transform.hpp>
#include <../../glm/glm/gtc/type_ptr.hpp>
#endif

#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include <string>

#include "classes//stb_image.h"

#include "classes//camera.h"
#include "classes//shaderCode_s.h"
#include "classes//meshCode.h"
#include "classes//modelCode.h"
#include "classes//animator.h"
#include "classes//player.h"

#include "classes//rigidbody.h"

// #include <ft2build.h>
// #include FT_FREETYPE_H

static void GLClearError()
{
    while(!glGetError());
}

static void GLCheckError()
{
    while (GLenum error = glGetError())
    {
        std::cout << "OpenGL error: " << error << std::endl;
    }
    
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1366; //1024; //1600; //1280; //800;
const unsigned int SCR_HEIGHT = 768; //1200; //720; //600;

// camera | forward/backward | up/down | left/right
Camera camera(glm::vec3(-3.0f, 1.75f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const glm::vec3 GRAVITY(0.0f, -9.81f, 0.0f);
std::vector<RigidBody> Rigidbodies;

void addRigidBody(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, float mass, bool is_static, bool is_animated, const Model& model) {
    RigidBody body(mass, position, rotation, scale, is_static, is_animated, model);
    body.model = model;
    // if (is_animated && animator) {
    //     body.animator = animator; // Assign animator if provided
    // }
    Rigidbodies.push_back(body);
}


// /// Holds all state information relevant to a character as loaded using FreeType
// struct Character {
//     unsigned int TextureID; // ID handle of the glyph texture
//     glm::ivec2   Size;      // Size of glyph
//     glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
//     unsigned int Advance;   // Horizontal offset to advance to next glyph
// };

// std::map<GLchar, Character> Characters;
// unsigned int VAO, VBO;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    #ifdef _WIN32
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    #endif

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    // stbi_set_flip_vertically_on_load(true);
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    #ifdef _WIN32
        Shader ourShader("..\\src\\shader_texture_stuff\\model_loading.vs", "..\\src\\shader_texture_stuff\\model_loading.fs");

        Model ourModel("..\\src\\models\\arms_gun.obj");

        Model building_0("..\\src\\models\\buildings\\building.obj");
        Model sceneModel("..\\src\\scene\\scene.obj");
        Model boxModel("..\\src\\models\\box\\box.obj");

    #elif __APPLE__
        Shader ourShader("../src/shader_texture_stuff/animated_model_loading.vs", "../src/shader_texture_stuff/model_loading.fs");
        
        // Model gunModel("../src/gunAnimated/gun_idle.dae");
        // gun_animations["idle"] = new Animation("../src/gunAnimated/gun_idle.dae", &gunModel);
        // gun_animations["reload"] = new Animation("../src/gunAnimated/gun_reload.dae", &gunModel);
        Model gunModel("../src/models_animated/FPS_gun_animated/FPS_gun_idle.fbx");
        
        gun_animations["idle"] = new Animation("../src/models_animated/FPS_gun_animated/FPS_gun_idle.fbx", &gunModel);
        gun_animations["reload"] = new Animation("../src/models_animated/FPS_gun_animated/FPS_gun_reload.fbx", &gunModel);
        gun_animations["fire"] = new Animation("../src/models_animated/FPS_gun_animated/FPS_gun_fire.fbx", &gunModel);
        gun_animations["walk"] = new Animation("../src/models_animated/FPS_gun_animated/FPS_gun_walk.fbx", &gunModel);
        // gun_animations["reload"] = new Animation("../src/gunAnimated/gun_reload.dae", &gunModel);
       
        // Model guyModel("../src/dancingGuy/dancing_vampire.dae");
        // // guy_animations["single"] = new Animation("../src/dancingGuy/dancing_vampire.dae", &guyModel);
        // Animation idleAnimation("../src/dancingGuy/dancing_vampire.dae", &guyModel);
        // Animator guy_animator(&idleAnimation);
        // Model ourModel("../src/AnimatedCharacterModel/model_idle.dae");
        Model ourModel("../src/models_animated/FPS_arms_animated/FPS_arms_idle.fbx");
        
        // int armBoneIndex = ourModel.GetBoneIndex("Armature_bicep_L");
        model_animations["idle"] = new Animation("../src/models_animated/FPS_arms_animated/FPS_arms_idle.fbx", &ourModel);
        model_animations["reload"] = new Animation("../src/models_animated/FPS_arms_animated/FPS_arms_reload.fbx", &ourModel);
        model_animations["fire"] = new Animation("../src/models_animated/FPS_arms_animated/FPS_arms_fire.fbx", &ourModel);
        model_animations["walk"] = new Animation("../src/models_animated/FPS_arms_animated/FPS_arms_walk.fbx", &ourModel);
        // model_animations["idle"] = new Animation("../src/AnimatedCharacterModel/model_idle.dae", &ourModel);
        // model_animations["reload"] = new Animation("../src/AnimatedCharacterModel/model_reload.dae", &ourModel);
        
        Model enemyModel("../src/models/enemy/source/poza.fbx");
        Model building_0("../src/models/buildings/building.obj");
        Model sceneModel("../src/scene/scene.obj");
        Model boxModel("../src/models/box/box.obj");

        
        // std::string font_name = "../../src/fonts/comic.ttf";
    #endif

    // pitch - x, yaw - y, roll - z
    // addRigidBody(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 50.0f, false, false, Model(), true, false); // camera
    // addRigidBody(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.01f), 70.0f, false, false, enemyModel, false, false);
    // addRigidBody(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.1f), 70.0f, true, false, building_0, false, false);
    // addRigidBody(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(glm::radians(-camera.Pitch - 5.0f), glm::radians(-camera.Yaw + 90.0f), 0.0f), glm::vec3(0.005f), 60.0f, false, true, ourModel, false, true, currentAnimatorModel);
    // addRigidBody(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(glm::radians(-camera.Pitch - 5.0f), glm::radians(-camera.Yaw + 90.0f), 0.0f), glm::vec3(0.005f), 60.0f, false, true, gunModel, false, true, currentAnimatorGun);
    addRigidBody(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 70.0f, true, false, sceneModel);
    addRigidBody(glm::vec3(-1.0f, 3.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 70.0f, false, false, boxModel);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    
        // activate shader
        ourShader.use();

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);


        for (int i = 0; i < Rigidbodies.size(); i++)
        {
            RigidBody& body = Rigidbodies[i];
            if (!body.is_static)
            {
                body.applyForce(GRAVITY);
                body.update(deltaTime);
            }
            
            glm::vec3 updatedPosition = body.position;

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, updatedPosition); // Translate to updated position
            modelMatrix = glm::rotate(modelMatrix, glm::radians(body.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
            modelMatrix = glm::rotate(modelMatrix, glm::radians(body.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
            modelMatrix = glm::rotate(modelMatrix, glm::radians(body.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll
            modelMatrix = glm::scale(modelMatrix, body.scale);   // Adjust scale if necessary

            ourShader.setMat4("model", modelMatrix);

            body.model.Draw(ourShader); // Use the model associated with this rigid body
            // }

            // std::cout << Rigidbodies.size() << std::endl;
        }

        glm::mat4 gunModelMatrix = glm::mat4(1.0f);
        ourShader.setBool("isAnimated", false);
        glm::vec3 cameraPosWithOffset = camera.Position - glm::vec3(0.0f, 0.00f, 0.00f);
        
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cameraPosWithOffset); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(-camera.Yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-camera.Pitch - 5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.005f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        // player.ProcessKeyboard(FORWARD_P, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        // player.ProcessKeyboard(BACKWARD_P, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
        // player.ProcessKeyboard(LEFT_P, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        // player.ProcessKeyboard(RIGHT_P, deltaTime);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
    // player.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
   
}
