#pragma once
#include "Core.h"
#include "Image.h"
#include "stb_image.h"
#include "Device.h"
#include "Buffer.h"

namespace Ajisai {
	class Texture
	{
	public:
		Image mTexImage;
		VkSampler mSampler;

		void CreateTexture(Device* device, const std::string filePath);
		void CleanUp(Device* device);
	};
}


