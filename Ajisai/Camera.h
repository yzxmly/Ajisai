#pragma once
#include "Core.h"

namespace Ajisai {
	class Camera
	{
	public:
		void Initialize();
		void Update();

		// camera related matrix
		glm::mat4 mMatToCam;
		glm::mat4 mMatToFrustum;

		// perspective controls
		float mFov, mAspect, mNearClip, mFarClip;

		// camera position
		glm::vec3 mCamPos, mCamLookup, mCamUp;

	};

}

