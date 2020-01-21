#include "Camera.h"

namespace Ajisai {
	void Camera::Initialize() {
		mFov = glm::radians(45.0f);
		mAspect = 1280.0f / 720.0f;
		mNearClip = 0.1f;
		mFarClip = 200.0f;

		mCamPos = glm::vec3(15.0f, 15.0f, 15.0f);
		mCamLookup = glm::vec3(0.0f, 8.0f, 0.0f);
		mCamUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void Camera::Update() {
		mMatToCam = glm::lookAt(mCamPos, mCamLookup, mCamUp);
		mMatToFrustum = glm::perspective(mFov, mAspect, mNearClip, mFarClip);
	}
}