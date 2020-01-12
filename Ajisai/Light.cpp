#include "Light.h"

namespace Ajisai {
	void Light::GenerateMatToLight() {
		glm::vec3 lookAt = mPosition + mDirection;
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		mMatToLight = glm::lookAt(mPosition, lookAt, up);
		mMatToFrustrum = glm::perspective(glm::radians(80.0f), 1.0f, 1.0f, 40.0f);

		mMatLight = mMatToFrustrum * mMatToLight;
	}
}
