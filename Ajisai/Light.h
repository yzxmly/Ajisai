#pragma once
#include "Core.h"

namespace Ajisai {
	class Light
	{
	public:
		glm::vec3 mPosition;
		glm::vec3 mDirection;

		glm::mat4 mMatToLight;
		glm::mat4 mMatToFrustrum;

		glm::mat4 mMatLight;
		void GenerateMatToLight();
	};
}


