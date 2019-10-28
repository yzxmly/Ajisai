#pragma once
#include "Core.h"
#include "Device.h"

namespace Ajisai {
	class Image
	{
	public:
		VkImage mImage;
		VkImageView mImageView;
		VkDeviceMemory mImageMemory;

		void CreateImage(Device *device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
		void CreateImageView(Device *device, VkFormat format, VkImageAspectFlags aspectFlags);
		void CleanUp(Device *device);
	
	private:
		static uint32_t FindMemoryType(Device *device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};

}
