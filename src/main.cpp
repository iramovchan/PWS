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
#include "classes//game_object.h"
#include "classes//enemy.h"

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

// camera | left/right | up/down | forward/backward
Camera camera(glm::vec3(0.0f, 2.0f, 7.0f));
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
std::vector<GameObject> gameObjects;
std::vector<RigidBody> Rigidbodies;
Player player;



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

void addRigidBody(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, glm::vec3 velocity, float mass, bool is_static, bool is_animated, std::string tag, Model* model) {
    RigidBody body(mass, position, rotation, scale, velocity, is_static, is_animated, tag, model);
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

struct Bullet {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::mat4 modelMatrix;  // To store the model transformation matrix
    bool hasRigidbody = false;
    // int rigidbodyIndex = -1;
};

std::vector<Bullet> bullets;  // Store all the bullets

void spawnBullet(glm::vec3 spawnPosition, glm::vec3 direction) {
    Bullet newBullet;
    newBullet.position = spawnPosition;
    // newBullet.velocity = direction * 10.0f;  // Adjust speed as needed
    newBullet.velocity = camera.Front * 15.0f;

    // Initialize the model matrix for the bullet (you can add rotations if needed)
    newBullet.modelMatrix = glm::mat4(1.0f);
    newBullet.modelMatrix = glm::translate(newBullet.modelMatrix, newBullet.position);

    
    bullets.push_back(newBullet);
}


void updateBullets(float deltaTime) {
    for (size_t i = 0; i < bullets.size(); i++) {
        if (bullets[i].hasRigidbody) {
            bullets[i].position = Rigidbodies[i].position;  // Use the loop index
        }
        // bullets[i].position += bullets[i].velocity * deltaTime;

        // Update the model matrix with the new position
        bullets[i].modelMatrix = glm::mat4(1.0f);
        bullets[i].modelMatrix = glm::translate(bullets[i].modelMatrix, bullets[i].position);
    }
}

// RigidBody cameraRB(70.0f, camera.Position, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), false, true, false, &Model());

// Rigidbodies.push_back(playerRB);

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
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Render wireframe


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
        
        // Model enemyModel("../src/models/enemy/source/poza.fbx");
        Model building_0("../src/models/buildings/building.obj");
        Model sceneModel("../src/scene/bank.obj");
        Model boxModel("../src/models/box/box.obj");
        Model bulletModel("../src/models/bullet.obj");
        Model armsStatic("../src/models/arms_gun.obj");
        Model playerAABBModel("../src/models/playerAABB.obj");
        Model enemyModel("../src/models/enemy.obj");

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
    addRigidBody(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), 70.0f, true, false, "scene", &sceneModel);
    // addRigidBody(glm::vec3(-1.0f, 3.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 70.0f, false, false, &boxModel);
    // addRigidBody(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), 100.0f, false, false, &bulletModel);

    glm::vec3 playerPos = camera.Position - glm::vec3(0.0f, 0.00f, 0.0f);
    glm::vec3 playerRot = glm::vec3(glm::radians(-camera.Pitch - 5.0f), glm::radians(-camera.Yaw + 90.0f), 0.0f);
    // glm::vec3 playerRot = glm::vec3(0.0f);
    glm::vec3 playerScale = glm::vec3(1.0f);

    // RigidBody playerRB(70.0f, glm::vec3(0.0f, 0.0f, 7.0f), glm::vec3(glm::radians(-camera.Pitch - 5.0f), glm::radians(-camera.Yaw + 90.0f), 0.0f), 1.0f, false, &ourModel);
    // Rigidbodies.push_back(cameraRB);
    // RigidBody playerAABBRB(70.0f, camera.Position - glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), false, true, "aabbcube", &playerAABBModel);
    // Rigidbodies.push_back(playerAABBRB);
    // RigidBody playerRB(70.0f, camera.Position, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), false, true, "player", &ourModel);

    // Rigidbodies.push_back(playerRB);
    // std::cout << Rigidbodies.size() << std::endl;
    // glfwSetWindowUserPointer(window, &playerRB);
    // gunModelMatrix = glm::rotate(gunModelMatrix, glm::radians(-camera.Yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //     gunModelMatrix = glm::rotate(gunModelMatrix, glm::radians(-camera.Pitch - 5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // render loop
    // -----------
    bool collisionFound = false;

    // glm::vec3 aabbMin = glm::vec3(-0.1f, -2.0f, -0.1f);
    // glm::vec3 aabbMax = glm::vec3(0.1f, 0.0f, 0.1f);
    glm::vec3 aabbMin = glm::vec3(-0.2f, -2.0f, -0.2f);
    glm::vec3 aabbMax = glm::vec3(0.2f, 0.0f, 0.2f);
        // 0.005
    // glm::vec3 aabbMin = glm::vec3(-20.0f, -400.0f, -20.0f);
    // glm::vec3 aabbMax = glm::vec3(20.0f, 0.0f, 20.0f);

    aiAABB playerAABB;
    playerAABB.mMin = {aabbMin.x, aabbMin.y, aabbMin.z};
    playerAABB.mMax = {aabbMax.x, aabbMax.y, aabbMax.z};
    std::cout << playerAABB.mMin.x << ", " << playerAABB.mMin.y << ", " << playerAABB.mMin.z << ", " << std::endl;

    // std::cout << ourModel.meshes.size() << std::endl;
    // std::cout << gunModel.meshes.size() << std::endl;
    ourModel.meshes[0].setupAABB(playerAABB);
    gunModel.meshes[0].setupAABB(playerAABB);
    // std::cout << ourModel.meshes.size() << std::endl;
    // std::cout << gunModel.meshes.size() << std::endl;

    auto collistion_checker = [&](const GameObject &go, glm::vec3 p){
        for(auto &body : Rigidbodies) {
            if(body.tag == "aabbcube") continue;
            std::pair<glm::vec3, glm::vec3> aabb = {p-glm::vec3(0.2, 1.5, 0.2), p+glm::vec3(0.2, 0, 0.2)};
            if(body.checkCollision(aabb)) {
                std::cout << "===== camera collides with " << body.tag << std::endl;
                return true;
            }
        }
        return false;
    };

    camera.check_collision = collistion_checker;

    // std::vector<Enemy> enemies;

    std::vector<glm::vec3> enemyPositions = {
        glm::vec3(0.0f, 0.3f, 0.0f),  // Position 1
        // glm::vec3(2.0f, 0.3f, 2.0f),  // Position 2
        // glm::vec3(-3.0f, 0.3f, 5.0f), // Position 3
        // glm::vec3(4.0f, 0.3f, -3.0f), // Position 4
        // glm::vec3(-6.0f, 0.3f, -6.0f) // Position 5
    };
    // for (const auto& enemy : enemies){
    //     enemy.check_collision = [&](const glm::vec3 &p){
    //         if
    //     }
    // }

    // for (const auto& position : enemyPositions) {
    //     enemies.push_back(Enemy(position));  // Create and add to the vector of enemies
    //     addRigidBody(position, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f), 50.0f, false, false, "enemy", &enemyModel);
    // }

    // for(int i = 0; i < 5; i++){
    //     Enemy newEnemy();
    // }

    // for(int enemy = 0; enemy < enemies.size(); enemy++) {
    //     for(auto &body : enemies) {
    //         std::pair<glm::vec3, glm::vec3> enemyAABB = {}
    //     }
    // }
    
    glm::vec3 newPlayerPos = playerPos; // + glm::vec3(0.0f, 2.0f, 0.0f);
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


        // if bullet rigidbody haven't been added?
        
        for (size_t i = 0; i < bullets.size(); i++) {
            if(bullets[i].hasRigidbody == false) {
                // addRigidBody(camera.Position + camera.Front, glm::vec3(camera.Yaw, camera.Pitch, 0.0f), glm::vec3(1.0f), 5.0f, false, false, &bulletModel);
                addRigidBody(bullets[i].position, glm::vec3(0.0f), glm::vec3(1.0f), bullets[i].velocity, 10.0f, false, false, "bullet", &bulletModel);
                bullets[i].hasRigidbody = true;
            }
        }
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

        // ourModel.meshes

        
        for (size_t i = 0; i < Rigidbodies.size(); ++i) {
            RigidBody& body = Rigidbodies[i];

            // Handle collisions with other rigid bodies
            // for (size_t j = i + 1; j < Rigidbodies.size(); ++j) {
            //     if (body.IsColliding(Rigidbodies[j])) {
            //         // collisionFound = true;
            //         std::cout << "collision checked num.: " << j << "i: " << i << std::endl;
            //         body.resolveAABBCollision(Rigidbodies[j]);
            //     }
            // }
                        // Apply physics updates if the body is not static

            if (!body.is_static) {
                // body.applyForce(GRAVITY);
                body.update(deltaTime);
                // for (auto& enemiesToMove : enemies) {
                //     enemiesToMove.update();
                //     body.position = enemiesToMove.position;
                //     std::cout << "enemyUpdated" << std::endl;
                // }
                
                // if(body.tag == "enemy" + i){
                //     enemies[i].update();
                // }
            }

            // if (body.tag == "aabbcube") {
            //     // std::cout << w"its the player" << std::endl;
            //     body.position = camera.Position - glm::vec3(0.0f, 1.9f, 0.0f);
            //     std::cout << body.position.x << ", " << body.position.y << ", " << body.position.z << std::endl;
            // }

            
                // Check collisions
            for (size_t j = i + 1; j < Rigidbodies.size(); ++j) {
              if (body.checkCollision(Rigidbodies[j])) {
                    // std::cout << "Rigidbody[" << i << "], checked with [" << j << "]" << std::endl;
                    // std::cout << "COLLISION!!!" << std::endl;
                    if (body.tag == "aabbcube") {
                        std::cout << "its a player collision" << std::endl;
                        camera.Position = body.position + glm::vec3(0.0f, 1.5f, 0.0f);
                    }
                    
                    // if(body.tag == "enemy") {
                    //     enemies[0].changeDirection();
                    // }
                    body.resolveCollision(Rigidbodies[j]);

                }
                else {
                    if (body.tag == "aabbcube") {
                        body.position = camera.Position - glm::vec3(0.0f, 1.5f, 0.0f);
                    }
                    
                }
            }

            // if (body.tag == "aabbcube") {
            //     // std::cout << w"its the player" << std::endl;
            //     body.position = camera.Position - glm::vec3(0.0f, 1.9f, 0.0f);
            //     std::cout << body.position.x << ", " << body.position.y << ", " << body.position.z << std::endl;
            // }

            if (body.is_animated == false) {
                // Calculate the model matrix
                glm::mat4 modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::translate(modelMatrix, body.position); // Translate to updated position
                modelMatrix = glm::rotate(modelMatrix, glm::radians(body.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
                modelMatrix = glm::rotate(modelMatrix, glm::radians(body.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
                modelMatrix = glm::rotate(modelMatrix, glm::radians(body.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll
                modelMatrix = glm::scale(modelMatrix, body.scale); // Adjust scale if necessary

                // Pass the model matrix to the shader
                ourShader.setMat4("model", modelMatrix);
                ourShader.setBool("isAnimated", body.is_animated);


                body.model->Draw(ourShader);
                body.model->DrawBoundingBox(ourShader);
            }
        }

        ourShader.setBool("isAnimated", true);
        auto gun_transforms = currentAnimatorGun->GetFinalBoneMatrices();
        for (int i = 0; i < gun_transforms.size(); ++i)
        {
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", gun_transforms[i]);
        }

        glm::mat4 gunModelMatrix = glm::mat4(1.0f);
        glm::vec3 newPlayerPosWithOffset = newPlayerPos;// + glm::vec3(0.0f, 2.0f, 0.0f);
        glm::vec3 cameraPosWithOffset = camera.Position - glm::vec3(0.0f, 0.00f, 0.0f);
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
        // glm::mat4 aabbModel = glm::mat4(1.0f);
        model = glm::translate(model, cameraPosWithOffset); // translate it down so it's at the center of the scene
        // aabbModel = glm::translate(aabbModel, cameraPosWithOffset);
        ourShader.setMat4("model", model);
        ourModel.DrawBoundingBox(ourShader);
        model = glm::rotate(model, glm::radians(-camera.Yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-camera.Pitch - 5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.005f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glm::vec3 playerBBposition = cameraPosWithOffset - glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 playerSize = glm::vec3(1.0f, 1.0f, 1.0f);

        ourShader.setBool("isAnimated", false);
        updateBullets(deltaTime);
        for (size_t i = 0; i < bullets.size(); i++) {
            // glm::mat4 model = glm::mat4(1.0f);
            // // Additional transformations for the main object (like camera translation, etc.)
            // model = glm::translate(model, cameraPosWithOffset); 
            // ourShader.setMat4("model", model);
            // ourModel.DrawBoundingBox(ourShader);

            glm::mat4 bulletModelMatrix = bullets[i].modelMatrix;
            bulletModelMatrix = glm::scale(bulletModelMatrix, glm::vec3(1.0f));  // scale it down if needed

            // std::cout << "BULLET position: " << bullets[0].position.x << ", " << bullets[0].position.y << ", " << bullets[0].position.z << std::endl;
            // std::cout << "AMOUNT OF BULLETS: " << bullets.size() << std::endl;
            
            // Set the model matrix for this bullet and draw it
            ourShader.setMat4("model", bulletModelMatrix);
            bulletModel.Draw(ourShader);  // You can use instanced drawing here
            bulletModel.DrawBoundingBox(ourShader);
        }
        // player.drawPlayerBoundingBox(ourShader, playerBBposition, playerSize);
        // player.drawPlayerBoundingBox(ourModel.meshes[0], gunModel.meshes[0], playerBBposition);

        // Player player;
        // Camera camera(glm::vec3(0.0f, 2.0f, 7.0f));
        

        // // Camera camera(glm::vec3(-3.0f, 1.75f, 0.0f));
        // glm::vec3 playerPosition = glm::vec3(0.0f, 1.0f, 0.0f);

        // player.drawPlayerBoundingBox(ourShader, playerPosition, playerSize);
        // forward/backward | up/down | left/right

        // camera.Position = playerRB.position + glm::vec3(0.0f, 2.0f, 0.0f);
        // camera.updateViewMAtrix();

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
bool bulletSpawned = false;
glm::vec3 playerMovement(0.0f);
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    bool moving = false;

    // RigidBody* playerRB = static_cast<RigidBody*>(glfwGetWindowUserPointer(window));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

   
    float movementSpeed = 5.0f;
    glm::vec3 moveForce(0.0f);


    // movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        // moveForce += glm::vec3(0.0f, 0.0f, -1.0f); // Move forward (assuming Z+ is forward)
        // playerMovement.z -= 1.0f; // Move forward
        camera.ProcessKeyboard(FORWARD, deltaTime);
        // player.ProcessKeyboard(FORWARD_P, deltaTime);
        moving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        // moveForce += glm::vec3(0.0f, 0.0f, 1.0f);  // Move backward (Z-)
        // playerMovement.z += 1.0f; // Move backward
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        // player.ProcessKeyboard(BACKWARD_P, deltaTime);
        moving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        // moveForce += glm::vec3(-1.0f, 0.0f, 0.0f); // Move left (X-)
        // playerMovement.x -= 1.0f; // Move left
        camera.ProcessKeyboard(LEFT, deltaTime);
        // player.ProcessKeyboard(LEFT_P, deltaTime);
        moving = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        // moveForce += glm::vec3(1.0f, 0.0f, 0.0f);  // Move right (X+)
        // playerMovement.x += 1.0f; // Move right
        camera.ProcessKeyboard(RIGHT, deltaTime);
        // player.ProcessKeyboard(RIGHT_P, deltaTime);
        moving = true;
    }

    // if (moving)
    // {
    //     // Normalize movement to avoid diagonal speed increase
    //     playerMovement = glm::normalize(playerMovement);

    //     // Apply the movement to the rigid body's velocity
    //     // playerRB.applyForce()
    //     playerRB->applyForce(playerMovement * movementSpeed);  // movementSpeed can be a predefined value
    // }

    //     // Movement controls
    // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    // {
    //     playerRB->applyForce(camera.Front * movementSpeed * deltaTime); // Move forward
    //     moving = true;
    // }
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    // {
    //     playerRB->applyForce(-camera.Front * movementSpeed * deltaTime); // Move backward
    //     moving = true;
    // }
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    // {
    //     playerRB->applyForce(-camera.Right * movementSpeed * deltaTime); // Move left
    //     moving = true;
    // }
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    // {
    //     playerRB->applyForce(camera.Right * movementSpeed * deltaTime); // Move right
    //     moving = true;
    // }
    
    // cameraRB.applyForce(moveForce * 10.0f);

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
        if(bulletSpawned == false) {
            spawnBullet(camera.Position + camera.Front, glm::vec3(camera.Yaw, camera.Pitch, 0.0f));
            bulletSpawned = true;
        }
        
        SwitchAnimation(model_animations, "fire", currentAnimatorModel, true, "idle");
        SwitchAnimation(gun_animations, "fire", currentAnimatorGun, true, "idle");
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        bulletSpawned = false;
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
