#pragma once

#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"
#include "bone.h"


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

class Animator
{
public:
	Animator(Animation* animation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;

};


// #pragma once

// #include <map>
// #include <vector>
// #include <assimp/scene.h>
// #include <assimp/Importer.hpp>
// #include "animation.h"
// #include "bone.h"


// #ifdef _WIN32
// #include <glad/glad.h> // holds all OpenGL type declarations
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include "shaderCode.h"
// #elif defined(__APPLE__)
// #include <OpenGL/gl3.h>  // macOS OpenGL
// #include <../../glm/glm/glm.hpp>
// #include <../../glm/glm/gtc/matrix_transform.hpp>
// #include "shaderCode.h"
// #endif

// class Animator
// {
// public:
//     Animator(Animation* animation)
//     {
//         m_CurrentTime = 0.0;
//         m_CurrentAnimation = animation;

//         // Initialize m_FinalBoneMatrices with 100 identity matrices by default.
//         m_FinalBoneMatrices = std::vector<glm::mat4>(100, glm::mat4(1.0f));
//     }

//     void UpdateAnimation(float dt)
//     {
//         m_DeltaTime = dt;
//         if (m_CurrentAnimation)
//         {
//             m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
//             m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
//             CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
//         }
//     }

//     void PlayAnimation(Animation* pAnimation)
//     {
//         m_CurrentAnimation = pAnimation;
//         m_CurrentTime = 0.0f;
//     }

//     void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
//     {
//         std::string nodeName = node->name;
//         glm::mat4 nodeTransform = node->transformation;

//         // Find the bone associated with the node
//         Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

//         // If bone is found, update its transformation
//         if (Bone)
//         {
//             Bone->Update(m_CurrentTime);
//             nodeTransform = Bone->GetLocalTransform();
//         }

//         glm::mat4 globalTransformation = parentTransform * nodeTransform;

//         // Retrieve the bone ID map and check if the node has a corresponding bone
//         auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
//         if (boneInfoMap.find(nodeName) != boneInfoMap.end())
//         {
//             int index = boneInfoMap[nodeName].id;
//             glm::mat4 offset = boneInfoMap[nodeName].offset;

//             // // Ensure the final bone matrices vector is large enough to accommodate the index
//             // if (index >= m_FinalBoneMatrices.size())
//             // {
//             //     std::cerr << "Warning: Bone index out of bounds! Resizing the matrix vector." << std::endl;
//             //     m_FinalBoneMatrices.resize(index + 1, glm::mat4(1.0f)); // Resize if necessary
//             // }

//             // Update the final bone matrix for this bone
//             m_FinalBoneMatrices[index] = globalTransformation * offset;
//         }

//         // Recursively process all the child nodes of the current node
//         for (int i = 0; i < node->childrenCount; i++)
//         {
//             CalculateBoneTransform(&node->children[i], globalTransformation);
//         }
//     }

//     std::vector<glm::mat4> GetFinalBoneMatrices()
//     {
//         return m_FinalBoneMatrices;
//     }

// private:
//     std::vector<glm::mat4> m_FinalBoneMatrices;
//     Animation* m_CurrentAnimation;
//     float m_CurrentTime;
//     float m_DeltaTime;
// };
