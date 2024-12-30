#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#define GLFW_INCLUDE_GLCOREARB
#ifdef _WIN32
#include <glad/glad.h>  // GLAD for Windows
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#elif defined(__APPLE__)
#include <OpenGL/gl3.h>  // macOS OpenGL
#include <../../glm/glm/glm.hpp>
#include <../../glm/glm/gtc/matrix_transform.hpp>
#include <../../glm/glm/gtc/type_ptr.hpp>
#endif

#include <GLFW/glfw3.h>
#include <iostream>


#include "classes//stb_image.h"

#include "classes//camera.h"
#include "classes//shaderCode_s.h"
#include "classes//meshCode.h"
#include "classes//modelCode.h"
#include "classes//animator.h"
#include "classes//player.h"

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
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera | left/right | up/down | forward/backward
Camera camera(glm::vec3(0.0f, 1.0f, 1.0f));
// glm::vec3 start_pos = glm::vec3(0.0f, 0.0f, 0.0f);
// Player player(start_pos);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    #ifdef _WIN32
        Shader ourShader("..\\..\\src\\shader_texture_stuff\\animated_model_loading.vs", "..\\..\\src\\shader_texture_stuff\\model_loading.fs");
        Model ourModel("..\\..\\src\\car_model\\ultrsalowpolycar.obj");
    #elif __APPLE__
        Shader ourShader("../src/shader_texture_stuff/animated_model_loading.vs", "../src/shader_texture_stuff/model_loading.fs");
        
        Model gunModel("../src/gunAnimated/gun_idle.dae");
        Animation idleAnimGun("../src/gunAnimated/gun_idle.dae", &gunModel);
        Animator gun_animator(&idleAnimGun);
        Model guyModel("../src/dancingGuy/dancing_vampire.dae");
        Animation idleAnimation("../src/dancingGuy/dancing_vampire.dae", &guyModel);
        Animator guy_animator(&idleAnimation);
        Model ourModel("../src/AnimatedCharacterModel/model_idle.dae");
        Animation idleAnimCharac("../src/AnimatedCharacterModel/model_idle.dae", &ourModel);
        Animator animator(&idleAnimCharac);
    #endif
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
        gun_animator.UpdateAnimation(deltaTime);
        guy_animator.UpdateAnimation(deltaTime);
        animator.UpdateAnimation(deltaTime);
        
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


        auto gun_transforms = gun_animator.GetFinalBoneMatrices();
        for (int i = 0; i < gun_transforms.size(); ++i)
        {
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", gun_transforms[i]);
        }

        glm::mat4 gunModelMatrix = glm::mat4(1.0f);
        gunModelMatrix = glm::translate(gunModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // Adjust position
        gunModelMatrix = glm::scale(gunModelMatrix, glm::vec3(0.5f));                 // Adjust scale
        ourShader.setMat4("model", gunModelMatrix);
        gunModel.Draw(ourShader);


        auto guy_transforms = guy_animator.GetFinalBoneMatrices();
        for (int i = 0; i < guy_transforms.size(); ++i)
        {
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", guy_transforms[i]);
        }

        glm::mat4 guyModelMatrix = glm::mat4(1.0f);
        guyModelMatrix = glm::translate(guyModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f)); // Adjust position
        guyModelMatrix = glm::scale(guyModelMatrix, glm::vec3(0.5f));                 // Adjust scale
        ourShader.setMat4("model", guyModelMatrix);
        guyModel.Draw(ourShader);

        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }

         // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
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

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Switch to wireframe mode
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Switch to normal mode
    } 
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
        // player.ProcessKeyboard(FORWARD_P, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        // player.ProcessKeyboard(BACKWARD_P, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
        // player.ProcessKeyboard(LEFT_P, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
        // player.ProcessKeyboard(RIGHT_P, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    //     camera.ProcessKeyboard(UP, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    //     camera.ProcessKeyboard(DOWN, deltaTime);
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
