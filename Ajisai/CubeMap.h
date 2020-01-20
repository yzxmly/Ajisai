#pragma once
#include "Core.h"
#include "Device.h"
#include "Buffer.h"
#include "stb_image.h"

namespace Ajisai {
	class CubeMap
	{
	public:
		VkImage mImage;
		VkImageView mImageView;
		VkDeviceMemory mImageMemory;
		VkFormat mFormat;

		VkSampler mSampler;

		void CreateCubeMap(Device* device, const std::string filePath);
		void CleanUp(Device* device);
	};

}

