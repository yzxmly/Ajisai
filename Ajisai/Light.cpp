#include "Light.h"

namespace Ajisai {
	void Light::GenerateMatToLight() {
		glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

		mMatToLight = glm::lookAt(mPosition, lookAt, up);
		mMatToFrustrum = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);

		mMatLight = mMatToFrustrum * mMatToLight;
	}
}