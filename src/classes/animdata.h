#pragma once

#ifdef _WIN32
#include <glm/glm.hpp>
#elif defined(__APPLE__)
#include <../../glm/glm/glm.hpp>
#endif

struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int id;

	/*offset matrix transforms vertex from model space to bone space*/
	glm::mat4 offset;

};
#pragma once