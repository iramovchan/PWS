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

	Animator ()
	{}

	Animator(Animation* animation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;
		m_PlayOnce = false;
		m_HasFinished = false;

		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void UpdateAnimation(float dt, std::map<std::string, Animation*> animations, const std::string& fallbackAnimation, Animator*& currentAnimator)
	{
		// m_CurrentAnimation, 
		m_DeltaTime = dt;
		if (m_CurrentAnimation && !m_HasFinished)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;

			if (m_PlayOnce && m_CurrentTime >= m_CurrentAnimation->GetDuration())
			{
				m_HasFinished = true;
				m_CurrentTime = m_CurrentAnimation->GetDuration();
				return;
			}
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
		else if (m_CurrentAnimation && m_PlayOnce && m_HasFinished)
		{
			
			if (animations.find(fallbackAnimation) != animations.end())
			{
				currentAnimator->PlayAnimation(animations[fallbackAnimation], false);
			}
			else if (fallbackAnimation == "single")
			{
				m_HasFinished = false;
				m_CurrentTime = 0.0f;
			}
			else
			{
				std::cout << "fallback animation was not found in animations" << std::endl;
			}
			

		}
		
	}

	void PlayAnimation(Animation* pAnimation, bool playOnce = false)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
		m_PlayOnce = playOnce;
		m_HasFinished = false;
	}

	bool HasFinished() const
	{
		return m_HasFinished;
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
	bool m_PlayOnce;
	bool m_HasFinished;
};
