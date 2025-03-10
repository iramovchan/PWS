#ifndef MESH_H
#define MESH_H

#ifdef _WIN32
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaderCode.h"
#elif defined(__APPLE__)
#include <OpenGL/gl3.h>  // macOS OpenGL
#include <../../glm/glm/glm.hpp>
#include <../../glm/glm/gtc/matrix_transform.hpp>
#include "shaderCode.h"
#endif

#include <assimp/aabb.h>

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    unsigned int aabbVAO = 0, aabbVBO = 0, aabbEBO = 0;
    bool aabbInitialized = false;
    aiAABB mAABB;
    // glm::vec3 aabbWorldMin; // Updated world-space AABB min
    // glm::vec3 aabbWorldMax; // Updated world-space AABB max
    glm::vec4 transform;


    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, const aiAABB &aabb)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->mAABB = aabb;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
        setupAABB(aabb);
    }

    // render the mesh
    void Draw(Shader &shader) 
    {
        // Activate the shader
        shader.use();  // <--- ADD THIS LINE
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++) 
        {
            glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
            string number;
            string name = textures[i].type;

            if(name == "texture_diffuse") {
                number = std::to_string(diffuseNr);  // Get current number
                diffuseNr++;  // Increment after assigning number
            } 
            else if(name == "texture_specular") {
                number = std::to_string(specularNr); 
                specularNr++; 
            } 
            else if(name == "texture_normal") {
                number = std::to_string(normalNr); 
                normalNr++; 
            } 
            else if(name == "texture_height") {
                number = std::to_string(heightNr); 
                heightNr++; 
            }

            // Now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // Bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }


        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        for (unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        }

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    // void DrawAABB(Shader& shader, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
    void DrawAABB(Shader& shader)
    {
        // if (aabbInitialized == true){
        //     return;
        // }
        // if (!aabbInitialized) {
        //     // Initialize AABB for this mesh
        //     aabbInitialized = true;
        // }

        shader.use();
        // shader.setMat4("model", model);
        // shader.setMat4("view", view);
        // shader.setMat4("projection", projection);

        glBindVertexArray(aabbVAO);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        // aabbInitialized = true;
    }

    void setupAABB(const aiAABB &aabb){

        if (aabbInitialized == false)
        {
            // std::cout << "FROM THE MESHCODE: AABBMIN: " << aabb.mMin.x << ", " << aabb.mMin.y << ", " << aabb.mMin.z << std::endl;   
            // std::cout << "FROM THE MESHCODE: AABBMAX: " << aabb.mMax.x << ", " << aabb.mMax.y << ", " << aabb.mMax.z << std::endl;   

            glm::vec3 vertices[8] = {
                {aabb.mMin.x, aabb.mMin.y, aabb.mMin.z},
                {aabb.mMax.x, aabb.mMin.y, aabb.mMin.z},
                {aabb.mMax.x, aabb.mMax.y, aabb.mMin.z},
                {aabb.mMin.x, aabb.mMax.y, aabb.mMin.z},
                {aabb.mMin.x, aabb.mMin.y, aabb.mMax.z},
                {aabb.mMax.x, aabb.mMin.y, aabb.mMax.z},
                {aabb.mMax.x, aabb.mMax.y, aabb.mMax.z},
                {aabb.mMin.x, aabb.mMax.y, aabb.mMax.z},
            };

            unsigned int indices[] = {
                0, 1, 1, 2, 2, 3, 3, 0,  // Bottom face
                4, 5, 5, 6, 6, 7, 7, 4,  // Top face
                0, 4, 1, 5, 2, 6, 3, 7   // Vertical edges
            };

            glGenVertexArrays(1, &aabbVAO);
            glGenBuffers(1, &aabbVBO);
            glGenBuffers(1, &aabbEBO);

            glBindVertexArray(aabbVAO);

            glBindBuffer(GL_ARRAY_BUFFER, aabbVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, aabbEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

            glBindVertexArray(0);
            // aabbInitialized = true;
        }
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
		// ids
		glEnableVertexAttribArray(5);
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

		// weights
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }
};
#endif