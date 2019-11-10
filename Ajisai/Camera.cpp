#include "Camera.h"

namespace Ajisai {
	void Camera::Initialize() {
		mFov = glm::radians(45.0f);
		mAspect = 800.0f / 600.0f;
		mNearClip = 0.1f;
		mFarClip = 100.0f;

		mCamPos = glm::vec3(13.0f, 13.0f, 13.0f);
		mCamLookup = glm::vec3(0.0f, 8.0f, 0.0f);
		mCamUp = glm::vec3(0.0f, 1.0f, 0.0f);

		mMatToCam = glm::lookAt(mCamPos, mCamLookup, mCamUp);
		mMatToFrustum = glm::perspective(mFov, mAspect, mNearClip, mFarClip);
	}

	void Camera::Update() {

	}
}