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

std::map<std::string, Animation*> model_animations;
std::map<std::string, Animation*> gun_animations;

Animator* currentAnimatorModel = new Animator(model_animations["idle"]);
Animator* currentAnimatorGun = new Animator(gun_animations["idle"]);

std::string currentAnimation = "idle";
const glm::vec3 GRAVITY(0.0f, -9.81f, 0.0f);
// const glm::vec3 BULLET_FORCE(0.0f, -9.81f, 0.0f);
std::vector<RigidBody> Rigidbodies;

void SwitchAnimation(std::map<std::string, Animation*> animations, const std::string& animationName, Animator*& currentAnimator, bool playOnce, const std::string& idleAnimation)
{
    if (animations.find(animationName) != animations.end())
    {
        delete currentAnimator; // Clean up the old animator
        
        currentAnimator = new Animator(animations[animationName]);
        if (currentAnimator->HasFinished() && animationName != idleAnimation)
        {
            // Automatically switch to idle if the current animation is finished
            std::cout << "back to idle" << std::endl;
            currentAnimator->PlayAnimation(animations[idleAnimation]);
            return;
        }
        currentAnimator->PlayAnimation(animations[animationName], playOnce);


        //         // Create a new animator for the desired animation
        // Animator* newAnimator = new Animator(animations[animationName]);

        // // Play the new animation
        // if (newAnimator->HasFinished() && animationName != idleAnimation)
        // {
        //     // Automatically switch to idle if the current animation is finished
        //     std::cout << "Switching back to idle" << std::endl;
        //     newAnimator->PlayAnimation(animations[idleAnimation]);
        // }
        // else
        // {
        //     newAnimator->PlayAnimation(animations[animationName], playOnce);
        // }

        // // Safely delete the old animator and assign the new one
        // delete currentAnimator;
        // currentAnimator = newAnimator;
    }
    else
    {
        std::cout << "Animation " << animationName << " not found!" << std::endl;
    }
}

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
        Shader ourShader("..\\src\\shader_texture_stuff\\animated_model_loading.vs", "..\\src\\shader_texture_stuff\\model_loading.fs");
        
        // Model gunModel("..\\src\\gunAnimated\\gun_idle.dae");
        // gun_animations["idle"] = new Animation("..\\src\\gunAnimated\\gun_idle.dae", &gunModel);
        // gun_animations["reload"] = new Animation("..\\src\\gunAnimated\\gun_reload.dae", &gunModel);
        Model gunModel("..\\src\\models_animated\\FPS_gun_animated\\fps_gun_idle.glb");
        
        gun_animations["idle"] = new Animation("..\\src\\models_animated\\FPS_gun_animated\\fps_gun_idle.glb", &gunModel);
        gun_animations["reload"] = new Animation("..\\src\\models_animated\\FPS_gun_animated\\fps_gun_reload.glb", &gunModel);
        gun_animations["fire"] = new Animation("..\\src\\models_animated\\FPS_gun_animated\\fps_gun_fire.glb", &gunModel);
        gun_animations["walk"] = new Animation("..\\src\\models_animated\\FPS_gun_animated\\fps_gun_walk.glb", &gunModel);
        // gun_animations["reload"] = new Animation("..\\src\\gunAnimated\\gun_reload.dae", &gunModel);
       
        // Model guyModel("..\\src\\dancingGuy\\dancing_vampire.dae");
        // \\\\ guy_animations["single"] = new Animation("..\\src\\dancingGuy\\dancing_vampire.dae", &guyModel);
        // Animation idleAnimation("..\\src\\dancingGuy\\dancing_vampire.dae", &guyModel);
        // Animator guy_animator(&idleAnimation);
        // Model ourModel("..\\src\\AnimatedCharacterModel\\model_idle.dae");
        Model ourModel("..\\src\\models_animated\\FPS_arms_animated\\fps_arms_idle.glb");
        
        // int armBoneIndex = ourModel.GetBoneIndex("Armature_bicep_L");
        model_animations["idle"] = new Animation("..\\src\\models_animated\\FPS_arms_animated\\fps_arms_idle.glb", &ourModel);
        model_animations["reload"] = new Animation("..\\src\\models_animated\\FPS_arms_animated\\fps_arms_reload.glb", &ourModel);
        model_animations["fire"] = new Animation("..\\src\\models_animated\\FPS_arms_animated\\fps_arms_fire.glb", &ourModel);
        model_animations["walk"] = new Animation("..\\src\\models_animated\\FPS_arms_animated\\fps_arms_walk.glb", &ourModel);
        // model_animations["idle"] = new Animation("..\\src\\AnimatedCharacterModel\\model_idle.dae", &ourModel);
        // model_animations["reload"] = new Animation("..\\src\\AnimatedCharacterModel\\model_reload.dae", &ourModel);
        
        Model enemyModel("..\\src\\models\\enemy\\source\\poza.glb");
        Model building_0("..\\src\\models\\buildings\\building.obj");
        Model sceneModel("..\\src\\scene\\bank.obj");
        Model boxModel("..\\src\\models\\box\\box.obj");

        
        // std::string font_name = "..\\..\\src\\fonts\\comic.ttf";
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
        Model sceneModel("../src/scene/bank.obj");
        Model boxModel("../src/models/box/box.obj");
        Model bulletModel("../src/models/bullet.obj");
        
        // std::string font_name = "../../src/fonts/comic.ttf";
    #endif

    // // FreeType
    // // --------
    // FT_Library ft;
    // // All functions return a value different than 0 whenever an error occurred
    // if (FT_Init_FreeType(&ft))
    // {
    //     std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    //     return -1;
    // }

    // if (font_name.empty())
    // {
    //     std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
    //     return -1;
    // }
	
	// // load font as face
    // FT_Face face;
    // if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
    //     std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    //     return -1;
    // }
    // else {
    //     // set size to load glyphs as
    //     FT_Set_Pixel_Sizes(face, 0, 48);

    //     // disable byte-alignment restriction
    //     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //     // load first 128 characters of ASCII set
    //     for (unsigned char c = 0; c < 128; c++)
    //     {
    //         // Load character glyph 
    //         if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    //         {
    //             std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    //             continue;
    //         }
    //         // generate texture
    //         unsigned int texture;
    //         glGenTextures(1, &texture);
    //         glBindTexture(GL_TEXTURE_2D, texture);
    //         glTexImage2D(
    //             GL_TEXTURE_2D,
    //             0,
    //             GL_RED,
    //             face->glyph->bitmap.width,
    //             face->glyph->bitmap.rows,
    //             0,
    //             GL_RED,
    //             GL_UNSIGNED_BYTE,
    //             face->glyph->bitmap.buffer
    //         );
    //         // set texture options
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //         // now store character for later use
    //         Character character = {
    //             texture,
    //             glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
    //             glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
    //             static_cast<unsigned int>(face->glyph->advance.x)
    //         };
    //         Characters.insert(std::pair<char, Character>(c, character));
    //     }
    //     glBindTexture(GL_TEXTURE_2D, 0);
    // }
    // // destroy FreeType once we're finished
    // FT_Done_Face(face);
    // FT_Done_FreeType(ft);

    
    // // configure VAO/VBO for texture quads
    // // -----------------------------------
    // glGenVertexArrays(1, &VAO);
    // glGenBuffers(1, &VBO);
    // glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
    SwitchAnimation(model_animations, "idle", currentAnimatorModel, false, "idle");
    SwitchAnimation(gun_animations, "idle", currentAnimatorGun, false, "idle");

    // pitch - x, yaw - y, roll - z
    // addRigidBody(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 50.0f, false, false, Model(), true, false); // camera
    // addRigidBody(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.01f), 70.0f, false, false, enemyModel, false, false);
    // addRigidBody(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.1f), 70.0f, true, false, building_0, false, false);
    // addRigidBody(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(glm::radians(-camera.Pitch - 5.0f), glm::radians(-camera.Yaw + 90.0f), 0.0f), glm::vec3(0.005f), 60.0f, false, true, ourModel, false, true, currentAnimatorModel);
    // addRigidBody(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(glm::radians(-camera.Pitch - 5.0f), glm::radians(-camera.Yaw + 90.0f), 0.0f), glm::vec3(0.005f), 60.0f, false, true, gunModel, false, true, currentAnimatorGun);
    addRigidBody(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 70.0f, true, false, sceneModel);
    addRigidBody(glm::vec3(-1.0f, 3.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 70.0f, false, false, boxModel);
    addRigidBody(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 1.0f, true, false, bulletModel);
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

        
        currentAnimatorModel->UpdateAnimation(deltaTime, model_animations, currentAnimation, currentAnimatorModel);
        currentAnimatorGun->UpdateAnimation(deltaTime, gun_animations, currentAnimation, currentAnimatorGun);
        
        // if (currentAnimatorModel != nullptr) {
        //     currentAnimatorModel->UpdateAnimation(deltaTime, model_animations, currentAnimation, currentAnimatorModel);
        //     std::cout << "animations are not nullpointers" << std::endl;
        // } else {
        //     std::cout << "currentAnimatorModel is nullptr!" << std::endl;
        // }

        // if (currentAnimatorGun != nullptr) {
        //     currentAnimatorGun->UpdateAnimation(deltaTime, gun_animations, currentAnimation, currentAnimatorGun);
        // } else {
        //     std::cout << "currentAnimatorGun is nullptr!" << std::endl;
        // }

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

            // if (body.camera)
            // {
            //     camera.Position = updatedPosition;
            // }
            // else
            // {
                // Handle camera-attached models
                // if (body.camera_attached) {
                //     updatedPosition = camera.Position;
                //     body.rotation.x = glm::radians(-camera.Pitch - 5.0f);
                //     body.rotation.y = glm::radians(-camera.Yaw + 90.0f);
                    
                //     // updatedPosition += glm::vec3(0.1f, -0.2f, -0.5f); // Gun offset
                //     // glm::vec3 cameraPosWithOffset = camera.Position - glm::vec3(0.0f, 0.00f, 0.00f);
                //     // std::cout << "camera attached" << std::endl;
                // }


            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, updatedPosition); // Translate to updated position
            modelMatrix = glm::rotate(modelMatrix, glm::radians(body.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
            modelMatrix = glm::rotate(modelMatrix, glm::radians(body.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
            modelMatrix = glm::rotate(modelMatrix, glm::radians(body.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll
            modelMatrix = glm::scale(modelMatrix, body.scale);   // Adjust scale if necessary

            ourShader.setMat4("model", modelMatrix);
            ourShader.setBool("isAnimated", body.is_animated);

                // if (body.is_animated && body.animator) {
                //     if (body.animator != nullptr)
                //     {
                        
                //         auto transforms = body.animator->GetFinalBoneMatrices();
                //         for (int i = 0; i < transforms.size(); ++i) {
                //             ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
                //         }

                //     }
                //     else
                //     {
                //         std::cout << "currentAnimator is null!" << std::endl;
                //     }

                    
                // }

            body.model.Draw(ourShader); // Use the model associated with this rigid body
            // }

            // std::cout << Rigidbodies.size() << std::endl;
        }


        ourShader.setBool("isAnimated", true);
        auto gun_transforms = currentAnimatorGun->GetFinalBoneMatrices();
        for (int i = 0; i < gun_transforms.size(); ++i)
        {
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", gun_transforms[i]);
        }

        glm::mat4 gunModelMatrix = glm::mat4(1.0f);
        glm::vec3 cameraPosWithOffset = camera.Position - glm::vec3(0.0f, 0.00f, 0.00f);
        // gunModelMatrix = glm::translate(gunModelMatrix, glm::vec3(0.09f, 0.7f, -0.26f)); // Adjust position
        gunModelMatrix = glm::translate(gunModelMatrix, cameraPosWithOffset); // Adjust position
        gunModelMatrix = glm::rotate(gunModelMatrix, glm::radians(-camera.Yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        gunModelMatrix = glm::rotate(gunModelMatrix, glm::radians(-camera.Pitch - 5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        gunModelMatrix = glm::scale(gunModelMatrix, glm::vec3(0.005f));                 // Adjust scale
        ourShader.setMat4("model", gunModelMatrix);
        gunModel.Draw(ourShader);

        auto transforms = currentAnimatorModel->GetFinalBoneMatrices();

        for (int i = 0; i < transforms.size(); ++i)
        {
            
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
        
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

    // Cleanup code
    for (auto& anim : model_animations)
    {
        delete anim.second; // Free each Animation*
    }
    for (auto& anim : gun_animations)
    {
        delete anim.second; // Free each Animation*
    }

    delete currentAnimatorModel;
    delete currentAnimatorGun;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    bool moving = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        // player.ProcessKeyboard(FORWARD_P, deltaTime);
        moving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        // player.ProcessKeyboard(BACKWARD_P, deltaTime);
        moving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
        // player.ProcessKeyboard(LEFT_P, deltaTime);
        moving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        // player.ProcessKeyboard(RIGHT_P, deltaTime);
        moving = true;
    }

    if(moving)
    {
        if (currentAnimation != "walk")
        {
            currentAnimation = "walk";
            SwitchAnimation(model_animations, "walk", currentAnimatorModel, false, currentAnimation);
            SwitchAnimation(gun_animations, "walk", currentAnimatorGun, false, currentAnimation);
        }
    }
    if (!moving) {
        if (currentAnimation != "idle")
        {
            currentAnimation = "idle";
            SwitchAnimation(model_animations, "idle", currentAnimatorModel, false, currentAnimation);
            SwitchAnimation(gun_animations, "idle", currentAnimatorGun, false, currentAnimation);
        }
    }

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        SwitchAnimation(model_animations, "reload", currentAnimatorModel, true, "idle");
        SwitchAnimation(gun_animations, "reload", currentAnimatorGun, true, "idle");
    }
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        SwitchAnimation(model_animations, "fire", currentAnimatorModel, true, "idle");
        SwitchAnimation(gun_animations, "fire", currentAnimatorGun, true, "idle");
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
